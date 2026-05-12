package main

import (
	"fmt"
	"net/http"
	"log"
)

func healthzHandler(w http.ResponseWriter, r *http.Request) {
	w.WriteHeader(http.StatusOK)
	fmt.Fprintln(w, "OK")
}

func targetingHandler(w http.ResponseWriter, r *http.Request) {
	w.WriteHeader(http.StatusOK)
	fmt.Fprintln(w, "Coordinates locked. Targeting sequence initiated.")
}

func main() {
	http.HandleFunc("/healthz", healthzHandler)
	http.HandleFunc("/target", targetingHandler)
	log.Println("Targeting service running on port 8080...")
	log.Fatal(http.ListenAndServe(":8080", nil))
}
