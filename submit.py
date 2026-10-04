import json

submit_args = {
    "branch_name": "mpga-enforce-format-changes",
    "commit_message": "mpga_reader: enforce channels and sample rate consistency mid-stream",
    "title": "Fix MPGA mid-stream format change handling",
    "description": "Enforce that the number of channels and sample rate do not change mid-stream in MPGA reader. Handling dynamic format changes is structurally difficult, so this treats any such changes as VC_CONTAINER_ERROR_FORMAT_NOT_SUPPORTED."
}
print(json.dumps(submit_args))
