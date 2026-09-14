import json
import os
import sys
import unittest
from io import StringIO
from unittest.mock import patch, mock_open

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.append(os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "scripts", "clive"))

from verify_roster import verify_dispatch

class TestVerifyRoster(unittest.TestCase):

    @patch("os.path.exists", return_value=False)
    @patch("sys.stdout", new_callable=StringIO)
    def test_config_missing(self, mock_stdout, mock_exists):
        with self.assertRaises(SystemExit) as cm:
            verify_dispatch()

        self.assertEqual(cm.exception.code, 1)
        self.assertIn("[FAIL] clive_roster.json not found.", mock_stdout.getvalue())

    @patch("os.path.exists", return_value=True)
    @patch("sys.stdout", new_callable=StringIO)
    def test_valid_roster(self, mock_stdout, mock_exists):
        sample_data = {
            "roster_name": "Test Roster",
            "total_agents": 2,
            "primary_agents": {
                "agent1": {"role": "Tester", "fallback": "agent2"},
                "agent2": {"role": "Backup", "fallback": "agent1"}
            }
        }
        m_open = mock_open(read_data=json.dumps(sample_data))
        with patch("builtins.open", m_open):
            verify_dispatch()

        output = mock_stdout.getvalue()
        self.assertIn("[OK] Roster Loaded: Test Roster (2 agents total)", output)
        self.assertIn("[VALID] AGENT1     | Role: Tester                 | Fallback -> AGENT2", output)
        self.assertIn("[VALID] AGENT2     | Role: Backup                 | Fallback -> AGENT1", output)
        self.assertIn("[SUCCESS] Clive Persona Roster dispatch routes verified.", output)

    @patch("os.path.exists", return_value=True)
    @patch("sys.stdout", new_callable=StringIO)
    def test_warning_fallback_outside_roster(self, mock_stdout, mock_exists):
        sample_data = {
            "roster_name": "Test Roster",
            "total_agents": 1,
            "primary_agents": {
                "agent1": {"role": "Tester", "fallback": "external_agent"}
            }
        }
        m_open = mock_open(read_data=json.dumps(sample_data))
        with patch("builtins.open", m_open):
            verify_dispatch()

        output = mock_stdout.getvalue()
        self.assertIn("[OK] Roster Loaded: Test Roster (1 agents total)", output)
        self.assertIn("[WARN] Agent 'agent1' fallback 'external_agent' is outside primary mapped roster.", output)
        self.assertIn("[SUCCESS] Clive Persona Roster dispatch routes verified.", output)

    @patch("os.path.exists", return_value=True)
    @patch("sys.stdout", new_callable=StringIO)
    def test_empty_roster(self, mock_stdout, mock_exists):
        sample_data = {
            "roster_name": "Empty Roster",
            "total_agents": 0
        }
        m_open = mock_open(read_data=json.dumps(sample_data))
        with patch("builtins.open", m_open):
            verify_dispatch()

        output = mock_stdout.getvalue()
        self.assertIn("[OK] Roster Loaded: Empty Roster (0 agents total)", output)
        self.assertIn("[SUCCESS] Clive Persona Roster dispatch routes verified.", output)

if __name__ == "__main__":
    unittest.main()
