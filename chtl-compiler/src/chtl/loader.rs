use std::fs;
use std::io;
use std::path::{Path, PathBuf};
use tempfile::{tempdir, TempDir};
use zip::ZipArchive;

#[derive(Debug)]
pub struct Loader {
    current_path: Option<PathBuf>,
    official_module_path: Option<PathBuf>,
    temp_dirs: Vec<TempDir>,
}

impl Loader {
    pub fn new() -> Self {
        Loader {
            current_path: None,
            official_module_path: None,
            temp_dirs: Vec::new(),
        }
    }

    pub fn set_current_file_path(&mut self, path: &str) {
        self.current_path = Some(PathBuf::from(path));
    }

    pub fn get_current_file_path(&self) -> Option<String> {
        self.current_path
            .as_ref()
            .and_then(|p| p.to_str().map(|s| s.to_string()))
    }

    pub fn set_official_module_path(&mut self, path: PathBuf) {
        self.official_module_path = Some(path);
    }

    pub fn resolve_path(&mut self, import_path_str: &str) -> io::Result<PathBuf> {
        let import_path = Path::new(import_path_str);

        let path_to_check = if import_path.is_absolute() {
            Some(import_path.to_path_buf())
        } else if import_path.starts_with("./") || import_path.starts_with("../") {
            self.current_path
                .as_ref()
                .and_then(|current| current.parent())
                .map(|parent| parent.join(import_path))
        } else {
            None
        };

        if let Some(path) = path_to_check {
            if path.exists() {
                return self.handle_cmod(path);
            }
        }

        let mut search_dirs = Vec::new();
        if let Some(official_path) = &self.official_module_path {
            search_dirs.push(official_path.clone());
        }
        if let Some(current) = &self.current_path {
            if let Some(parent) = current.parent() {
                search_dirs.push(parent.join("module"));
                search_dirs.push(parent.to_path_buf());
            }
        }

        let has_extension = import_path.extension().is_some();
        for dir in search_dirs {
            if !has_extension {
                let cmod_path = dir.join(import_path).with_extension("cmod");
                if cmod_path.exists() {
                    return self.handle_cmod(cmod_path);
                }
                let chtl_path = dir.join(import_path).with_extension("chtl");
                if chtl_path.exists() {
                    return self.handle_cmod(chtl_path);
                }
            } else {
                let full_path = dir.join(import_path);
                if full_path.exists() {
                    return self.handle_cmod(full_path);
                }
            }
        }

        Err(io::Error::new(
            io::ErrorKind::NotFound,
            format!("Module not found: {}", import_path_str),
        ))
    }

    fn handle_cmod(&mut self, path: PathBuf) -> io::Result<PathBuf> {
        if path.extension().and_then(|s| s.to_str()) == Some("cmod") {
            let temp_dir = tempdir()?;
            let file = fs::File::open(&path)?;
            let mut archive = ZipArchive::new(file)?;
            archive.extract(temp_dir.path())?;

            let module_name = path
                .file_stem()
                .and_then(|s| s.to_str())
                .unwrap_or_default();
            let entry_point = temp_dir.path().join("src").join(format!("{}.chtl", module_name));

            if !entry_point.exists() {
                return Err(io::Error::new(
                    io::ErrorKind::NotFound,
                    format!("CMOD entry point not found at {:?}", entry_point),
                ));
            }

            self.temp_dirs.push(temp_dir);
            Ok(entry_point)
        } else {
            Ok(path)
        }
    }

    pub fn load_file_content(&mut self, path_str: &str) -> io::Result<String> {
        let resolved_path = self.resolve_path(path_str)?;
        fs::read_to_string(resolved_path)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs::{self, File};
    use std::io::Write;
    use tempfile::tempdir;
    use zip::write::{FileOptions, ZipWriter};

    fn create_cmod_file(path: &Path, module_name: &str, content: &str) {
        let file = File::create(path).unwrap();
        let mut zip = ZipWriter::new(file);
        let options = FileOptions::default().compression_method(zip::CompressionMethod::Stored);

        zip.add_directory("src/", options).unwrap();
        zip.start_file(format!("src/{}.chtl", module_name), options)
            .unwrap();
        zip.write_all(content.as_bytes()).unwrap();
        zip.finish().unwrap();
    }

    #[test]
    fn test_resolve_direct_relative_path() {
        let dir = tempdir().unwrap();
        let current_file_path = dir.path().join("src/main.chtl");
        fs::create_dir_all(current_file_path.parent().unwrap()).unwrap();
        File::create(&current_file_path).unwrap();

        let lib_path = dir.path().join("src/lib.chtl");
        File::create(&lib_path).unwrap();

        let mut loader = Loader::new();
        loader.set_current_file_path(current_file_path.to_str().unwrap());

        let resolved = loader.resolve_path("./lib.chtl").unwrap();
        assert_eq!(resolved, lib_path);
    }

    #[test]
    fn test_load_file_content_not_found() {
        let mut loader = Loader::new();
        let result = loader.load_file_content("non_existent_file.chtl");
        assert!(result.is_err());
    }

    #[test]
    fn test_module_resolution_order() {
        let dir = tempdir().unwrap();

        let official_dir = dir.path().join("official_modules");
        fs::create_dir(&official_dir).unwrap();
        File::create(official_dir.join("MyMod.chtl")).unwrap();

        let proj_dir = dir.path().join("my_project/src");
        fs::create_dir_all(&proj_dir).unwrap();
        let proj_module_dir = proj_dir.parent().unwrap().join("module");
        fs::create_dir(&proj_module_dir).unwrap();
        File::create(proj_module_dir.join("MyMod.chtl")).unwrap();

        File::create(proj_dir.join("MyMod.chtl")).unwrap();

        let mut loader = Loader::new();
        loader.set_official_module_path(official_dir.clone());
        loader.set_current_file_path(proj_dir.join("main.chtl").to_str().unwrap());

        let resolved = loader.resolve_path("MyMod").unwrap();
        assert_eq!(resolved, official_dir.join("MyMod.chtl"));
    }

    #[test]
    fn test_cmod_over_chtl_preference() {
        let dir = tempdir().unwrap();
        let proj_dir = dir.path().join("my_project");
        fs::create_dir(&proj_dir).unwrap();

        let mut chtl_file = File::create(proj_dir.join("MyMod.chtl")).unwrap();
        writeln!(chtl_file, "/* chtl file */").unwrap();

        create_cmod_file(&proj_dir.join("MyMod.cmod"), "MyMod", "/* cmod file */");

        let mut loader = Loader::new();
        loader.set_current_file_path(proj_dir.join("main.chtl").to_str().unwrap());

        let resolved_path = loader.resolve_path("MyMod").unwrap();

        let content = fs::read_to_string(resolved_path).unwrap();
        assert_eq!(content, "/* cmod file */");
    }

    #[test]
    fn test_cmod_loading_and_extraction() {
        let dir = tempdir().unwrap();
        let cmod_path = dir.path().join("MyMod.cmod");
        create_cmod_file(&cmod_path, "MyMod", "div {}");

        let mut loader = Loader::new();
        let resolved_path = loader.resolve_path(cmod_path.to_str().unwrap()).unwrap();

        assert!(resolved_path.starts_with(loader.temp_dirs[0].path()));
        assert_eq!(resolved_path.file_name().unwrap(), "MyMod.chtl");

        let content = fs::read_to_string(resolved_path).unwrap();
        assert_eq!(content, "div {}");
    }
}