package algoritmos

var (
	threshhold int = 1000
)

func swap(arr []int, i int, j int) {
	temp := arr[i]
	arr[i] = arr[j]
	arr[j] = temp
}

func partition(arr []int, low int, high int) int {

	/* Optimização 2: mediana de 3 */
    mid := low + (high-low)/2
    if arr[low] > arr[mid] {
        swap(arr, low, mid)
    }
    if arr[low] > arr[high] {
        swap(arr, low, high)
    }
    if arr[mid] > arr[high] {
        swap(arr, mid, high)
    }
    swap(arr, mid, high)
    pivot := arr[high]

    i := low
    j := high - 1
    for {
        for i++; arr[i] < pivot; i++ {}
        for j--; arr[j] > pivot; j-- {}

        
        if i >= j {
            break
        }
        
		if (arr[i] == pivot) {
			
		}

        swap(arr, i, j)
    }
}


func quicksort(arr []int, low int, high int) {

	if low >= high || low < 0 {
		return
	}

	/* Optimzação 1: Insertion sort para len < threshhold */
	if ( (1+high-low) <= threshhold) {
		InsertionSortN(arr, low, high)
		return
	}

	pivot := partition(arr, low, high)

	quicksort(arr, low+1, pivot)
	quicksort(arr, pivot, high-1)

}

func QuickSort(arr []int) {
	quicksort(arr, 0, len(arr)-1)
}
