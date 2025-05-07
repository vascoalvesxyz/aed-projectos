package main

import (
	"log"
	"math/rand"
	"prj3/algoritmos"
	"sort"
	"time"
	"os"
	"io"
)

type ConjuntoGen struct {
	nome string
	fn 	 func(int)[]int 
}

type Algoritmo struct {
	nome string
	fn 	 func([]int)
}

var (
	media int = 10
	tamanhos = []int{1000, 10000, 100000, 250000, 500000, 750000, 1000000}
	conjuntos = []ConjuntoGen{
		{"Conjunto A", ConjuntoA},
		{"Conjunto B", ConjuntoB},
		{"Conjunto C", ConjuntoC},
	}
	algos = []Algoritmo{ 
		{"Insertion Sort", algoritmos.InsertionSort},
		{"Heap Sort", algoritmos.HeapSort},
		{"Quicksort", algoritmos.QuickSort},
	}
)

func gerarConjuntoRepetido(size int, repetir float64) ([]int) {
	if repetir > 1 || repetir < 0 {
		panic("[gerarConjuntoRepetido] repetir deve ser uma percentagem ente 0 e 1 válida")
	}

	base := make([]int, 0, size)
	numUnicos := int(float64(size) * repetir)
	numRepetidos := size - numUnicos

	for i := 0; i < numUnicos; i++ {
		base = append(base, i)
	}
	// Adicionar repetições aleatórias
	for i := 0; i < numRepetidos; i++ {
		valor := base[rand.Intn(len(base))]
		base = append(base, valor)
	}

	return base
}

func ConjuntoA(size int) ([]int) {
	arr := gerarConjuntoRepetido(size, 0.95)
	sort.Ints(arr)
	return arr
}

func ConjuntoB(size int) ([]int) {
	arr := gerarConjuntoRepetido(size, 0.95)
	sort.Sort(sort.Reverse(sort.IntSlice(arr)))
	return arr
}

func ConjuntoC(size int) ([]int) {
	arr := gerarConjuntoRepetido(size, 0.95)
	rand.Shuffle(len(arr), func(i, j int) {
		arr[i], arr[j] = arr[j], arr[i]
	})
	return arr
}

func TestAlgorithm(alg func([]int), conjGen func(int) ([]int), size int, attempts int) (time.Duration) {

	totalTime := time.Duration(0)

	conj := conjGen(size)

	for i := 0; i < attempts; i++ {
		arr := make([]int, len(conj))
		copy(arr, conj)

		start := time.Now()
		alg(conj)
		totalTime += time.Since(start)

		// log.Print(conj)
	}

	return totalTime / time.Duration(attempts) 
}

func main() {

	logFile, err := os.OpenFile("log.txt", os.O_CREATE | os.O_APPEND | os.O_RDWR, 0666)
	if err != nil {
		panic(err)
	}
	mw := io.MultiWriter(os.Stdout, logFile)
	log.SetOutput(mw)


	for _, tamanho := range tamanhos {
		for _, algoritmo := range algos {
			for _, conjunto := range conjuntos {
				res := TestAlgorithm(algoritmo.fn, conjunto.fn, tamanho, media)
				log.Printf("[RESULTADO]\t%s\t%s\tSIZE=%d\tAVG=%d\tMédia Final = %.3fms\n",
					algoritmo.nome, conjunto.nome, tamanho, media, float64(res)/float64(time.Millisecond))
			}
		}
	}

	log.Print("Done!\n")
}
