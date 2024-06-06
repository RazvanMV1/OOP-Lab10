#include <iostream>

using namespace std;

class OutOfRangeException : public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "Index out of range!";
    }
};

class Compare
{
public:
    virtual int CompareElements(void* e1, void* e2) = 0;
};

template<class T>
class ArrayIterator
{
private:
    int Current;

public:
    ArrayIterator() : Ptr(nullptr), CurrentIndex(0) {}

    ArrayIterator(T** ptr, int index) : Ptr(ptr), CurrentIndex(index) {}

    ArrayIterator(const ArrayIterator<T>& other) : Ptr(other.Ptr), CurrentIndex(other.CurrentIndex) {}

    ArrayIterator<T>& operator++()
    {
        ++CurrentIndex;
        return *this;
    }

    ArrayIterator<T>& operator--()
    {
        --CurrentIndex;
        return *this;
    }

    bool operator==(const ArrayIterator<T>& other) const
    {
        return Ptr == other.Ptr && CurrentIndex == other.CurrentIndex;
    }

    bool operator!=(const ArrayIterator<T>& other) const
    {
        return !(*this == other);
    }

    T& operator*() const
    {
        return *(Ptr[CurrentIndex]);
    }

    ArrayIterator<T>& operator=(const ArrayIterator<T>& other)
    {
        if (this != &other)
        {
            Ptr = other.Ptr;
            CurrentIndex = other.CurrentIndex;
        }
        return *this;
    }
};

template<class T>
class Array
{
private:
    T** List;
    int Capacity;
    int Size;

public:
    Array() : List(nullptr), Capacity(0), Size(0) {}

    ~Array()
    {
        if (List != nullptr) {
            for (int i = 0; i < Size; ++i) {
                delete List[i];
            }
            delete[] List;
        }
    }

    Array(int capacity) : Capacity(capacity), Size(0)
    {
        List = new T * [Capacity];
    }

    Array(const Array<T>& otherArray)
    {
        Capacity = otherArray.Capacity;
        Size = otherArray.Size;
        List = new T * [Capacity];
        for (int i = 0; i < Size; ++i) {
            List[i] = new T(*(otherArray.List[i]));
        }
    }

    T& operator[] (int index)
    {
        if (index < 0 || index >= Size) {
            throw OutOfRangeException();
        }
        return *(List[index]);
    }

    const Array<T>& operator+=(const T& newElem)
    {
        if (Size >= Capacity) {
            Capacity *= 2;
            T** newList = new T * [Capacity];
            for (int i = 0; i < Size; ++i) {
                newList[i] = List[i];
            }
            delete[] List;
            List = newList;
        }
        List[Size] = new T(newElem);
        ++Size;
        return *this;
    }

    const Array<T>& Insert(int index, const T& newElem)
    {
        if (index < 0 || index > Size) {
            throw OutOfRangeException();
        }
        if (Size >= Capacity) {
            Capacity *= 2;
            T** newList = new T * [Capacity];
            for (int i = 0; i < index; ++i) {
                newList[i] = List[i];
            }
            newList[index] = new T(newElem);
            for (int i = index; i < Size; ++i) {
                newList[i + 1] = List[i];
            }
            delete[] List;
            List = newList;
        }
        else {
            for (int i = Size; i > index; --i) {
                List[i] = List[i - 1];
            }
            List[index] = new T(newElem);
        }
        ++Size;
        return *this;
    }

    const Array<T>& Insert(int index, const Array<T> otherArray)
    {
        if (index < 0 || index > Size) {
            throw OutOfRangeException();
        }
        if (Size + otherArray.Size > Capacity) {
            Capacity = Size + otherArray.Size;
            T** newList = new T * [Capacity];
            for (int i = 0; i < index; ++i) {
                newList[i] = List[i];
            }
            for (int i = 0; i < otherArray.Size; ++i) {
                newList[index + i] = new T(*(otherArray.List[i]));
            }
            for (int i = index; i < Size; ++i) {
                newList[i + otherArray.Size] = List[i];
            }
            delete[] List;
            List = newList;
        }
        else {
            for (int i = Size - 1; i >= index; --i) {
                List[i + otherArray.Size] = List[i];
            }
            for (int i = 0; i < otherArray.Size; ++i) {
                List[index + i] = new T(*(otherArray.List[i]));
            }
        }
        Size += otherArray.Size;
        return *this;
    }

    const Array<T>& Delete(int index)
    {
        if (index < 0 || index >= Size) {
            throw OutOfRangeException();
        }
        delete List[index];
        for (int i = index; i < Size - 1; ++i) {
            List[i] = List[i + 1];
        }
        --Size;
        return *this;
    }

    bool operator=(const Array<T>& otherArray)
    {
        if (&otherArray == this) {
            return *this;
        }
        if (List != nullptr) {
            for (int i = 0; i < Size; ++i) {
                delete List[i];
            }
            delete[] List;
        }
        Capacity = otherArray.Capacity;
        Size = otherArray.Size;
        List = new T * [Capacity];
        for (int i = 0; i < Size; ++i) {
            List[i] = new T(*(otherArray.List[i]));
        }
        return *this;
    }

    void Sort()
    {
        // Bubble Sort implementation
        if (Size <= 1) return;
        for (int i = 0; i < Size - 1; ++i) {
            for (int j = 0; j < Size - i - 1; ++j) {
                if (*List[j] > *List[j + 1]) {
                    T* temp = List[j];
                    List[j] = List[j + 1];
                    List[j + 1] = temp;
                }
            }
        }
    }

    void Sort(int(*compare)(const T&, const T&))
    {
        // Sort with compare function implementation
        if (Size <= 1) return;
        for (int i = 0; i < Size - 1; ++i) {
            for (int j = 0; j < Size - i - 1; ++j) {
                if (compare(*List[j], *List[j + 1]) > 0) {
                    T* temp = List[j];
                    List[j] = List[j + 1];
                    List[j + 1] = temp;
                }
            }
        }
    }

    void Sort(Compare* comparator)
    {
        // Sort with comparator implementation
        if (Size <= 1) return;
        for (int i = 0; i < Size - 1; ++i) {
            for (int j = 0; j < Size - i - 1; ++j) {
                if (comparator->CompareElements(List[j], List[j + 1]) > 0) {
                    T* temp = List[j];
                    List[j] = List[j + 1];
                    List[j + 1] = temp;
                }
            }
        }
    }

    int BinarySearch(const T& elem)
    {
        // BinarySearch implementation
        int left = 0;
        int right = Size - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (*List[mid] == elem) return mid;
            if (*List[mid] < elem) left = mid + 1;
            else right = mid - 1;
        }
        return -1; // Not found
    }

    int BinarySearch(const T& elem, int(*compare)(const T&, const T&))
    {
        // BinarySearch with compare function implementation
        int left = 0;
        int right = Size - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            int comp = compare(*List[mid], elem);
            if (comp == 0) return mid;
            if (comp < 0) left = mid + 1;
            else right = mid - 1;
        }
        return -1; // Not found
    }

    int BinarySearch(const T& elem, Compare* comparator)
    {
        // BinarySearch with comparator implementation
        int left = 0;
        int right = Size - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            int comp = comparator->CompareElements(List[mid], &elem);
            if (comp == 0) return mid;
            if (comp < 0) left = mid + 1;
            else right = mid - 1;
        }
        return -1; // Not found
    }

    int Find(const T& elem)
    {
        // Find implementation
        for (int i = 0; i < Size; ++i) {
            if (*List[i] == elem) return i;
        }
        return -1; // Not found
    }

    int Find(const T& elem, int(*compare)(const T&, const T&))
    {
        // Find with compare function implementation
        for (int i = 0; i < Size; ++i) {
            if (compare(*List[i], elem) == 0) return i;
        }
        return -1; // Not found
    }

    int Find(const T& elem, Compare* comparator)
    {
        // Find with comparator implementation
        for (int i = 0; i < Size; ++i) {
            if (comparator->CompareElements(List[i], &elem) == 0) return i;
        }
        return -1; // Not found
    }

    int GetSize()
    {
        return Size;
    }

    int GetCapacity()
    {
        return Capacity;
    }

    ArrayIterator<T> GetBeginIterator()
    {
        return ArrayIterator<T>(List, 0);
    }

    ArrayIterator<T> GetEndIterator()
    {
        return ArrayIterator<T>(List, Size);
    }

};


class exceptie1 : public exception
{
    virtual const char* what() const throw()
    {
        return "Impartire la 0!";
    }
};

class exceptie2 : public exception
{
    virtual const char* what() const throw()
    {
        return "Indexul este inafara domeniului!";
    }
};

int main()
{
    int z = -1, x = 50, y = 0;
    int arr[4] = { 0 };
    int i = 10;
    exceptie1 divide0;
    exceptie2 index_out_of_bounds;

    try
    {
        if (y == 0)
            throw divide0;
        z = x / y;
        cout << "Fara exceptie: z=" << z << endl;
    }
    catch (exception& e)
    {
        cout << "Exceptie: " << e.what() << endl;
    }

    try
    {
        if (i > 3)
            throw index_out_of_bounds;
        arr[i] = z;
        for (i = 0; i < 4; i++)
            cout << "arr[" << i << "] = " << arr[i] << endl;
    }
    catch (exception& e)
    {
        cout << "Exceptie: " << e.what() << endl;
    }

    return 0;
}
