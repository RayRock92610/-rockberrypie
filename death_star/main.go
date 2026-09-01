package main

import (
"log"
"net/http"
"time"
)

func main() {
mux := http.NewServeMux()

server := &http.Server{
            ":8080",
dler:           mux,
* time.Second,
     10 * time.Second,
    10 * time.Second,
     30 * time.Second,
}

log.Fatal(server.ListenAndServe())
}
