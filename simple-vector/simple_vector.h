#pragma once

#include <cassert>
#include <stdexcept>
#include<algorithm>
#include <initializer_list>
#include <iterator>
#include "array_ptr.h"

struct ReserveProxyObj {
    ReserveProxyObj(size_t capacity)
        :cap(capacity)
    {
    }
    size_t cap;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        ArrayPtr<Type> numbers(size);
        std::generate(numbers.Get(), numbers.Get() + size, []() {return Type{};});
        elements_.swap(numbers);
        size_=size;
        capacity_=size;
    }
    
    SimpleVector(const ReserveProxyObj& obj){
     ArrayPtr<Type> numbers(obj.cap);     std::generate(numbers.Get(),numbers.Get()+obj.cap,[]() {return Type{}; });
        elements_.swap(numbers);
        capacity_=obj.cap;
    size_=0;
    }
    
    SimpleVector (const SimpleVector& other){
    ArrayPtr<Type> numbers(other.capacity_);
    std::copy(other.begin(),other.end(),numbers.Get());
    elements_.swap(numbers);
    size_=other.size_;
    capacity_=other.capacity_;
    }
    
    SimpleVector(SimpleVector&& other){
    swap(other);
    }
    
    SimpleVector(size_t size, const Type& value) {
       ArrayPtr<Type> numbers(size);
       std::fill(numbers.Get(),numbers.Get()+size, value);
       elements_.swap(numbers);
       size_=size;
       capacity_=size;
    }
    
     SimpleVector(size_t size, const Type&& value) {
       ArrayPtr<Type> numbers(size);
       std::generate(numbers.Get(),numbers.Get()+size, [value](){return value;});
       elements_.swap(numbers);
       size_=size;
       capacity_=size;
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        ArrayPtr<Type> numbers(init.size());
       std::copy(init.begin(),init.end(),numbers.Get());
        elements_.swap(numbers);
        size_=init.size();
        capacity_=init.size();
    }
    
    SimpleVector(std::initializer_list<Type>&& init) {
        ArrayPtr<Type> numbers(init.size());
       std::move(init.begin(),init.end(),numbers.Get());
        elements_.swap(numbers);
        size_=init.size();
        capacity_=init.size();
    }
    
    SimpleVector& operator= (const SimpleVector& rhs){
    if (this==&rhs) return *this;
    SimpleVector copy(rhs);
    swap(copy);
    return *this;
    }
    
    SimpleVector& operator= (SimpleVector&& rhs){
    if (this==&rhs) return *this;
    SimpleVector copy(rhs);
    swap(copy);
    return *this;
    }
    
   
  
    void Reserve(size_t new_capacity){
    if (new_capacity>capacity_){
    size_t temp_size=size_;
    SimpleVector copy(new_capacity);
    std::move(begin(),end(),copy.begin());
    swap(copy);
    size_=temp_size;
    }
    }
    
    void PushBack(const Type& item) {
        if (size_==capacity_) {
        size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
        SimpleVector copy (new_capacity);
        copy.size_=size_+1;
        std::copy(begin(),end(),copy.begin());
        
        swap(copy);
        *(begin()+size_-1)=item;
        return;
        }
        if (size_<capacity_){
        *(end())=item;
        ++size_;
        }
    }
    
    void PushBack(Type&& item) {
        if (size_==capacity_) {
        size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
        SimpleVector copy (new_capacity);
        copy.size_=size_+1;
        std::move(begin(),end(),copy.begin());
        
        swap(copy);
        *(begin()+size_-1)=std::move(item);
        return;
        }
        if (size_<capacity_){
        *(end())=std::move(item);
        ++size_;
        }
    }
    
    Iterator Insert(ConstIterator pos, const Type& value) {
         size_t index = pos - begin();
        if (size_ < capacity_) {
            if (pos == end()) {
                elements_[size_] = value;
               
            } else {
                std::copy_backward(elements_.Get() + index, elements_.Get() + size_, elements_.Get() + size_ + 1);
                elements_[index] = value;
            }
            ++size_;
        } else {
            size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
            ArrayPtr<Type> new_data (new_capacity);
            std::copy(elements_.Get(), elements_.Get() + index, new_data.Get());
            new_data[index] = value;
            std::copy(elements_.Get() + index, elements_.Get() + size_, new_data.Get() + index + 1);
            elements_.swap(new_data);
            capacity_ = new_capacity;
            ++size_;
        }
        return begin() + index;
    }
    
    Iterator Insert(ConstIterator pos,Type&& value) {
         size_t index = pos - begin();
        if (size_ < capacity_) {
            if (pos == end()) {
                elements_[size_] = std::move(value);
                
            } else {
                std::move_backward(elements_.Get() + index,elements_.Get() + size_,elements_.Get() + size_ + 1);
                elements_[index] = std::move(value);
            }
            ++size_;
        } else {
            size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
            ArrayPtr<Type> new_data (new_capacity);
            std::move(elements_.Get(), elements_.Get() + index, new_data.Get());
            new_data[index] = std::move(value);
            std::move(elements_.Get() + index, elements_.Get() + size_, new_data.Get() + index + 1);
            elements_.swap(new_data);
            capacity_ = new_capacity;
            ++size_;
        }
        return begin() + index;
    }
    
    void PopBack() noexcept {
        if (IsEmpty()) return;
        else {
        --size_;
        }
    }
    
     Iterator Erase(ConstIterator pos) {
     size_t index=pos-begin();
        std::move(begin()+index+1,end(),begin()+index);
        --size_;
        return (begin()+index);
    }
    
    void swap(SimpleVector& other) noexcept{
    elements_.swap(other.elements_);
    std::swap(other.size_,size_);
    std::swap(other.capacity_,capacity_);
    }
    
    void swap(SimpleVector&& other) noexcept{
    std::move(other.begin(),other.end(),begin());
    std::swap(other.size_,size_);
    std::swap(other.capacity_,capacity_);
    }


    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
       
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return (size_==0);
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
       return elements_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        return elements_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index>=size_) throw std::out_of_range("at");
        return elements_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
       if (index>=size_) throw std::out_of_range("at");
        return elements_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_=0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if(new_size<=capacity_){
        if (new_size<=size_)
        size_=new_size;
        if (new_size>size_){
        ArrayPtr<Type> numbers(new_size);
        std::move(elements_.Get(),elements_.Get()+size_,numbers.Get());
        std::generate(numbers.Get()+size_,numbers.Get()+new_size,[](){return Type{};});
        elements_.swap(numbers);
        size_=new_size;
        }
        }
        else {
        ArrayPtr<Type> numbers(new_size);
        std::move(elements_.Get(),elements_.Get()+size_,numbers.Get());
        std::generate(numbers.Get()+size_,numbers.Get()+new_size,[](){return Type{};});
        elements_.swap(numbers);
        size_=new_size;
        capacity_=new_size;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return elements_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
       return elements_.Get()+size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
       return elements_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
       return elements_.Get()+size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
         return elements_.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
       return elements_.Get()+size_;
    }
    private:
    ArrayPtr<Type> elements_{};
    size_t size_{0};
    size_t capacity_{0};
};

template <typename Type>
bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs){
if (lhs.GetSize()!=rhs.GetSize()) return false;
return std::equal(lhs.begin(),lhs.end(),rhs.begin());
}

template <typename Type>
bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs){
return !(lhs==rhs);
}

template <typename Type> 
bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) { 
    return std::lexicographical_compare(lhs.begin(),lhs.end(),rhs.begin(),rhs.end()); 
}

template <typename Type> 
bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) { 

    return !(lhs>rhs); 
} 

template <typename Type> 

bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) { 

    return (rhs<lhs); 

} 

 

template <typename Type> 

bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) { 

    return !(lhs<rhs); 

}  