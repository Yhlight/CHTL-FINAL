use std::fs;
use std::io;
use std::path::Path;

pub struct Loader;

impl Loader {
    pub fn new() -> Self {
        Loader
    }

    pub fn load_file_content<P: AsRef<Path>>(&self, path: P) -> io::Result<String> {
        fs::read_to_string(path)
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