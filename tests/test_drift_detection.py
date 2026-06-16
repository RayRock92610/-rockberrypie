import unittest
import os
import sys
from unittest.mock import patch, mock_open

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
import drift_detection

class TestDriftDetection(unittest.TestCase):
    @patch('builtins.open', new_callable=mock_open)
    def test_get_file_hash_ioerror(self, mock_file):
        mock_file.side_effect = IOError("Mocked IO Error")
        result = drift_detection.get_file_hash("some_dummy_file.txt")
        self.assertIsNone(result)

if __name__ == '__main__':
    unittest.main()
