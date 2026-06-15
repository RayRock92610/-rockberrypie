import unittest
import os
import sys
from unittest.mock import patch
from io import StringIO

# Append parent dir to path so we can import verify_kessel_context
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from verify_kessel_context import verify_kessel_context

class TestVerifyKesselContext(unittest.TestCase):
    @patch.dict(os.environ, {"KESSEL_ENV": "test", "KESSEL_MEMORY_DIR": "/tmp", "READ_ONLY_MODE": "1"})
    @patch('sys.stdout', new_callable=StringIO)
    def test_success(self, mock_stdout):
        # Should not raise any exception or call sys.exit
        verify_kessel_context()
        self.assertIn("[INFO] Kessel Memory routing initialized in test mode.", mock_stdout.getvalue())

    @patch.dict(os.environ, {"KESSEL_MEMORY_DIR": "/tmp", "READ_ONLY_MODE": "1"})
    @patch('sys.stderr', new_callable=StringIO)
    def test_missing_one_var(self, mock_stderr):
        # Temporarily clear KESSEL_ENV which we patched out
        if "KESSEL_ENV" in os.environ:
            del os.environ["KESSEL_ENV"]

        with self.assertRaises(SystemExit) as cm:
            verify_kessel_context()

        self.assertEqual(cm.exception.code, 1)
        self.assertIn("[FATAL] Missing Kessel constraints: KESSEL_ENV", mock_stderr.getvalue())

    @patch.dict(os.environ, {}, clear=True)
    @patch('sys.stderr', new_callable=StringIO)
    def test_missing_all_vars(self, mock_stderr):
        with self.assertRaises(SystemExit) as cm:
            verify_kessel_context()

        self.assertEqual(cm.exception.code, 1)
        self.assertIn("[FATAL] Missing Kessel constraints: KESSEL_ENV, KESSEL_MEMORY_DIR, READ_ONLY_MODE", mock_stderr.getvalue())

if __name__ == '__main__':
    unittest.main()
