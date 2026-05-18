import os, tempfile, pytest, drift_detection
def test_is_excluded():
    exclusions = {"dirs": [".git"], "files": ["*.log"]}
    assert drift_detection.is_excluded("test.log", exclusions) == True
