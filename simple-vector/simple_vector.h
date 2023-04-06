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

    explicit SimpleVector(size_t size) : size_(size), capacity_(size){
        ArrayPtr<Type> numbers(size);
        std::generate(numbers.Get(), numbers.Get() + size, []() {return Type{};});
        elements_.swap(numbers);
    }
    
    SimpleVector(const ReserveProxyObj& obj) :  size_(0), capacity_(obj.cap){
     ArrayPtr<Type> numbers(obj.cap);   
        elements_.swap(numbers);
    }
    
    SimpleVector (const SimpleVector& other) : size_(other.size_), capacity_(other.capacity_){
    ArrayPtr<Type> numbers(other.capacity_);
    std::copy(other.begin(),other.end(),numbers.Get());
    elements_.swap(numbers);
    }
    
    SimpleVector(SimpleVector&& other){
    swap(other);
    }
    
    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size) {
       ArrayPtr<Type> numbers(size);
       std::fill(numbers.Get(),numbers.Get()+size, value);
       elements_.swap(numbers);
    }
    
    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()) {
        ArrayPtr<Type> numbers(init.size());
       std::copy(init.begin(),init.end(),numbers.Get());
        elements_.swap(numbers);
    }
    
    SimpleVector(std::initializer_list<Type>&& init) : size_(init.size()), capacity_(init.size()) {
        ArrayPtr<Type> numbers(init.size());
       std::move(init.begin(),init.end(),numbers.Get());
        elements_.swap(numbers);
    }
    
    SimpleVector& operator= (const SimpleVector& rhs){
    if (this==&rhs) return *this;
    SimpleVector copy(rhs);
    swap(copy);
    return *this;
    }
    
    SimpleVector& operator= (SimpleVector&& rhs){
    if (this==&rhs) return *this;
    swap(rhs);
    return *this;
    }
    
    void Reserve(size_t new_capacity){
    if (new_capacity>capacity_){
    capacity_=new_capacity;
    ArrayPtr<Type> raw_ptr(new_capacity);
    std::move(begin(),end(),raw_ptr.Get());
    elements_.swap(raw_ptr);
    }
    }
    
    void PushBack(const Type& item) {
        if (size_==capacity_) {
        size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
        Reserve(new_capacity);
        ++size_;
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
        Reserve(new_capacity);
        ++size_;
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
     ForInsert(pos,index);
        elements_[index] = value;
        return begin() + index;
    }
    
    Iterator Insert(ConstIterator pos,Type&& value) {
    size_t index = pos - begin();
    ForInsert(pos,index);
        elements_[index] = std::move(value);
        return begin() + index;
    }
     
    void PopBack() noexcept {
        assert(!IsEmpty());
        --size_;
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
    
    size_t GetSize() const noexcept {
        
        return size_;
    }

    size_t GetCapacity() const noexcept {
       
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return (size_==0);
    }

    Type& operator[](size_t index) noexcept {
       return elements_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return elements_[index];
    }

    Type& At(size_t index) {
        if (index>=size_) throw std::out_of_range("at");
        return elements_[index];
    }

    const Type& At(size_t index) const {
       if (index>=size_) throw std::out_of_range("at");
        return elements_[index];
    }

    void Clear() noexcept {
        size_=0;
    }

    void Resize(size_t new_size) {
    if (new_size<=size_){
    size_=new_size;
    return;
    }
     Reserve(new_size);
     std::generate(elements_.Get()+size_,elements_.Get()+new_size,[](){return Type{};});
        size_=new_size;
        capacity_=new_size;
    }

    Iterator begin() noexcept {
        return elements_.Get();
    }

    Iterator end() noexcept {
       return elements_.Get()+size_;
    }

    ConstIterator begin() const noexcept {
       return elements_.Get();
    }

    ConstIterator end() const noexcept {
       return elements_.Get()+size_;
    }

    ConstIterator cbegin() const noexcept {
         return elements_.Get();
    }

    ConstIterator cend() const noexcept {
       return elements_.Get()+size_;
    }
    private:
    ArrayPtr<Type> elements_{};
    size_t size_{0};
    size_t capacity_{0};
    
    void ForInsert(ConstIterator pos, size_t index){
        if (size_ < capacity_) {
            if (pos == end()) {
            } else {
                std::move_backward(elements_.Get() + index, elements_.Get() + size_, elements_.Get() + size_ + 1); 
            }
            ++size_;
        } else {
            size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
            Reserve(new_capacity);
            std::move(elements_.Get() + index, elements_.Get() + size_, elements_.Get() + index + 1);
            capacity_ = new_capacity;
            ++size_;
        }
    }
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