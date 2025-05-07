package algoritmos

func InsertionSortN(arr []int, start int, end int) {
	/* clamp end idx */
	if end >= len(arr)-1 {
		end = len(arr)-1
	}
	/* clamp start idx */
	if start < 0 {
		start = 0
	}

	for i := start+1; i <= end ; i++ {
		key := arr[i]

		j := i - 1
		for j >= 0 && arr[j] > key {
			arr[j+1] = arr[j]
			j--
		}

		arr[j+1] = key
	}
}

func InsertionSort(arr []int) {
	InsertionSortN(arr, 0, len(arr)-1)
}
