## 2026-10-04 - mmal: implement clock port connection functionality
**Learning:** In MMAL graph, clock port connection functionality was disabled via assigning the function pointer to `NULL`. The solution involves checking if the underlying graph clock port defines a `pf_connect` function. If it does, we assign the `graph_port_connect` function to the component's clock port `pf_connect` pointer, utilizing the existing port proxying pattern.
**Action:** Next time when implementing a "disabled for now" port function in the MMAL framework (e.g., `pf_connect`), verify whether a proxy mechanism (like `graph_port_connect`) exists, check if the underlying port supports it via a presence check (e.g., `graph->clock[i]->priv->pf_connect`), and assign the proxy function accordingly. Be sure to strip out "disabled" conditions (e.g., `&& 0`) to actually enable the feature.

## 2026-10-04 - [Testing] Add edge case test for canonicalize
**Learning:** Adding test case to cover array canonicalization and handling of nulls/undefined to prevent regressions in custom canonicalize method logic that implements JSON stringification requirements.
**Action:** Next time when custom JSON serialization optimizations are implemented, add tests targeting specific edge cases like `null`, `undefined`, nested objects and arrays handling, and check how they adhere to standard JSON stringify behavior.
