package algoritmos

var (
	threshhold int = 1000
)

func swap(arr []int, i int, j int) {
	arr[i], arr[j] = arr[j], arr[i]
}

func partition(arr []int, low int, high int) (int,int) {

	/* Optimização 3: mediana de 3 */
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

	/* Optimização 4: dutch national flag */
	i := low      // primeiro elmento
	j := low      // elemento atual
	k := high - 1 // ultimo elemento

	for j <= k {
		if arr[j] < pivot {
			swap(arr, i, j)
			i++
			j++
		} else if arr[j] > pivot {
			swap(arr, j, k)
			k--
		} else {
			j++
		}
	}

	swap(arr, j, high)
	return i, j
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

	/* Optimiazação 2 */
	left, right := partition(arr, low, high)
	quicksort(arr, low, left-1)   // Sort elements less than pivot
	quicksort(arr, right+1, high) // Sort elements greater than pivot
}

func QuickSort(arr []int) {
	quicksort(arr, 0, len(arr)-1)
}
