def partition(array, low, high):  # 100, 150, 151

    print("0\n151\n1\n" + str(100+high) + "\n2\n152")
    pivot = array[high]  # piv:152

    print("3\n150\n4\n153")
    i = low - 1  # i:153

    for j in range(low, high):  # j:154, rang:160+
        print("4\n154\n" + str(160+low) + "\n5")

        print("6\n154\n" + str(100 + j) + "\n7\n152")
        if array[j] <= pivot:

            print("8\n153\n153")
            i = i + 1

            print("9\n154\n" + str(100+j) + "\n10\n153\n" + str(100+i))
            print("11\n" + str(100+i) + "\n" + str(100+j))
            (array[i], array[j]) = (array[j], array[i])

    print("12\n151\n" + str(100+high) + "\n13\n153\n" + str(100+i+1))
    print("14\n" + str(100+i+1) + "\n" + str(100+high))
    (array[i + 1], array[high]) = (array[high], array[i + 1])

    print("15\n153")
    return i + 1


def quickSort(array, low, high):  # 100, 150, 151

    print("16\n150\n151\n17")
    if low < high:

        print("18\n150\n19\n151")
        pi = partition(array, low, high)  # pi:155
        print("20\n155")

        print("21\n150\n22\n155")
        quickSort(array, low, pi - 1)

        print("23\n155\n24\n151")
        quickSort(array, pi + 1, high)


data = [3, 2, 4, 1]
print("Unsorted Array\n", data)

size = len(data)
quickSort(data, 0, size - 1)

print('Sorted Array in Ascending Order:\n', data)
