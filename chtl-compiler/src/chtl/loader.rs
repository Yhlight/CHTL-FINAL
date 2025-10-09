use std::fs;
use std::io;
use std::path::{Path, PathBuf};

#[derive(Debug, Clone)]
pub struct Loader {
    current_path: Option<PathBuf>,
}

impl Loader {
    pub fn new() -> Self {
        Loader {
            current_path: None,
        }
    }

    pub fn set_current_file_path(&mut self, path: &str) {
        self.current_path = Some(PathBuf::from(path));
    }

    pub fn load_file_content<P: AsRef<Path>>(&self, path: P) -> io::Result<String> {
        let path_to_load = if let Some(current) = &self.current_path {
            if let Some(parent) = current.parent() {
                parent.join(path)
            } else {
                path.as_ref().to_path_buf()
            }
        } else {
            path.as_ref().to_path_buf()
        };
        fs::read_to_string(path_to_load)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs::File;
    use std::io::Write;
    use tempfile::tempdir;

    #[test]
    fn test_load_file_content_success() {
        let dir = tempdir().unwrap();
        let file_path = dir.path().join("test.chtl");
        let mut file = File::create(&file_path).unwrap();
        writeln!(file, "div {{ text: \"hello\" }}").unwrap();

        let loader = Loader::new();
        let content = loader.load_file_content(&file_path).unwrap();
        assert_eq!(content.trim(), "div { text: \"hello\" }");
    }

    #[test]
    fn test_load_file_content_not_found() {
        let loader = Loader::new();
        let result = loader.load_file_content("non_existent_file.chtl");
        assert!(result.is_err());
        assert_eq!(result.err().unwrap().kind(), io::ErrorKind::NotFound);
    }
}