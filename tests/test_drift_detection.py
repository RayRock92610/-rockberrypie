import unittest
import os
import sys
import tempfile
import shutil
import json
import hashlib
from unittest.mock import patch, mock_open

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import drift_detection

class TestDriftDetection(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.mkdtemp()

        # Create some files
        self.file1 = os.path.join(self.temp_dir, "file1.txt")
        self.file2 = os.path.join(self.temp_dir, "file2.txt")
        self.log_file = os.path.join(self.temp_dir, "test.log")

        os.makedirs(os.path.join(self.temp_dir, "dir1"))
        self.file3 = os.path.join(self.temp_dir, "dir1", "file3.txt")

        os.makedirs(os.path.join(self.temp_dir, "excluded_dir"))
        self.file4 = os.path.join(self.temp_dir, "excluded_dir", "file4.txt")

        for p in [self.file1, self.file2, self.file3, self.file4, self.log_file]:
            with open(p, "w") as f:
                f.write(f"Content of {os.path.basename(p)}")

        self.exclusions = {"dirs": ["excluded_dir"], "files": ["*.log", "baseline.json"]}

        self.original_baseline_file = drift_detection.BASELINE_FILE
        drift_detection.BASELINE_FILE = os.path.join(self.temp_dir, "baseline.json")

    def tearDown(self):
        shutil.rmtree(self.temp_dir)
        drift_detection.BASELINE_FILE = self.original_baseline_file
        # clear regex cache
        drift_detection._CACHE.clear()

    @patch('builtins.open', new_callable=mock_open)
    def test_get_file_hash_ioerror(self, mock_file):
        mock_file.side_effect = IOError("Mocked IO Error")
        result = drift_detection.get_file_hash("some_dummy_file.txt")
        self.assertIsNone(result)



    def test_get_file_hash_success_small(self):
        # Test file smaller than BUFFER_SIZE
        test_file = os.path.join(self.temp_dir, "hash_test_small.txt")
        content = b"Small test content"
        with open(test_file, "wb") as f:
            f.write(content)

        expected_hash = hashlib.sha256(content).hexdigest()
        result = drift_detection.get_file_hash(test_file)
        self.assertEqual(result, expected_hash)

    def test_get_file_hash_success_large(self):
        # Test file larger than BUFFER_SIZE by temporarily reducing BUFFER_SIZE
        test_file = os.path.join(self.temp_dir, "hash_test_large.txt")
        content = b"Large test content"
        with open(test_file, "wb") as f:
            f.write(content)

        original_buffer_size = drift_detection.BUFFER_SIZE
        drift_detection.BUFFER_SIZE = 4
        try:
            expected_hash = hashlib.sha256(content).hexdigest()
            result = drift_detection.get_file_hash(test_file)
            self.assertEqual(result, expected_hash)
        finally:
            drift_detection.BUFFER_SIZE = original_buffer_size

    def test_create_baseline(self):
        count = drift_detection.create_baseline(self.temp_dir, self.exclusions)
        self.assertEqual(count, 3)
        self.assertTrue(os.path.exists(drift_detection.BASELINE_FILE))
        with open(drift_detection.BASELINE_FILE, "r") as f:
            data = json.load(f)
        self.assertIn("file1.txt", data)
        self.assertIn("file2.txt", data)
        self.assertIn(os.path.join("dir1", "file3.txt"), data)
        self.assertNotIn("test.log", data)
        self.assertNotIn(os.path.join("excluded_dir", "file4.txt"), data)
        self.assertNotIn("baseline.json", data)


    def test_create_baseline_empty_dir(self):
        empty_dir = os.path.join(self.temp_dir, "empty_dir")
        os.makedirs(empty_dir)
        count = drift_detection.create_baseline(empty_dir, self.exclusions)
        self.assertEqual(count, 0)
        self.assertTrue(os.path.exists(drift_detection.BASELINE_FILE))
        with open(drift_detection.BASELINE_FILE, "r") as f:
            data = json.load(f)
        self.assertEqual(data, {})

    def test_check_integrity_no_drift(self):
        drift_detection.create_baseline(self.temp_dir, self.exclusions)
        result, err = drift_detection.check_integrity(self.temp_dir, self.exclusions)
        self.assertIsNone(err)
        self.assertEqual(result["new"], [])
        self.assertEqual(result["deleted"], [])
        self.assertEqual(result["modified"], [])

    def test_check_integrity_with_drift(self):
        drift_detection.create_baseline(self.temp_dir, self.exclusions)

        # Modify file2.txt
        with open(self.file2, "a") as f:
            f.write("modified")

        # Delete file1.txt
        os.remove(self.file1)

        # Add new_file.txt
        new_file = os.path.join(self.temp_dir, "new_file.txt")
        with open(new_file, "w") as f:
            f.write("New content")

        result, err = drift_detection.check_integrity(self.temp_dir, self.exclusions)
        self.assertIsNone(err)
        self.assertIn("new_file.txt", result["new"])
        self.assertIn("file1.txt", result["deleted"])
        self.assertIn("file2.txt", result["modified"])

    def test_check_integrity_missing_baseline(self):
        result, err = drift_detection.check_integrity(self.temp_dir, self.exclusions)
        self.assertIsNone(result)
        self.assertEqual(err, "Baseline missing")


    def test_is_excluded_with_dict(self):
        exclusions = {"dirs": ["ignored_dir"], "files": ["*.tmp"]}
        self.assertTrue(drift_detection.is_excluded("some/path/ignored_dir", "ignored_dir", exclusions))
        self.assertTrue(drift_detection.is_excluded("some/path/file.tmp", "file.tmp", exclusions))
        self.assertFalse(drift_detection.is_excluded("some/path/valid_dir", "valid_dir", exclusions))
        self.assertFalse(drift_detection.is_excluded("some/path/file.txt", "file.txt", exclusions))

    def test_is_excluded_with_tuple(self):
        exclusions = ("ignored_dir", "*.tmp")
        self.assertTrue(drift_detection.is_excluded("some/path/ignored_dir", "ignored_dir", exclusions))
        self.assertTrue(drift_detection.is_excluded("some/path/file.tmp", "file.tmp", exclusions))
        self.assertFalse(drift_detection.is_excluded("some/path/valid_dir", "valid_dir", exclusions))
        self.assertFalse(drift_detection.is_excluded("some/path/file.txt", "file.txt", exclusions))

if __name__ == '__main__':
    unittest.main()
