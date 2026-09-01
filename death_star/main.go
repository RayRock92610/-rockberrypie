package main

import (
"log"
"net/http"
"time"
)

func main() {
mux := http.NewServeMux()
mux.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
:= &http.Server{
            ":8080",
dler:           mux,
* time.Second,
     10 * time.Second,
    10 * time.Second,
     30 * time.Second,
}

log.Fatal(server.ListenAndServe())
}
