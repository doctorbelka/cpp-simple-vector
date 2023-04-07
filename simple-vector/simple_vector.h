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

    explicit SimpleVector(size_t size) : elements_(size), size_(size), capacity_(size){
        std::generate(elements_.Get(), elements_.Get() + size, []() {return Type{};});
    }
    
    SimpleVector(const ReserveProxyObj& obj) :  elements_(obj.cap), size_(0), capacity_(obj.cap){
    }
    
    SimpleVector (const SimpleVector& other) : elements_(other.capacity_), size_(other.size_), capacity_(other.capacity_){
    std::copy(other.begin(),other.end(),elements_.Get());
    }
    
    SimpleVector(SimpleVector&& other){
    swap(other);
    }
    
    SimpleVector(size_t size, const Type& value) : elements_(size), size_(size), capacity_(size) {
       std::fill(elements_.Get(),elements_.Get()+size, value);
    }
    
    SimpleVector(std::initializer_list<Type> init) : elements_(init.size()), size_(init.size()), capacity_(init.size()) {
       std::copy(init.begin(),init.end(),elements_.Get());
    }
    
    SimpleVector(std::initializer_list<Type>&& init) : elements_(init.size()), size_(init.size()), capacity_(init.size()) {
       std::move(init.begin(),init.end(),elements_.Get());
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
        } 
        *(end())=item; 
        ++size_;    
    }
    
    void PushBack(Type&& item) {
     if (size_==capacity_) { 
        size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_; 
        Reserve(new_capacity); 
        } 
        *(end())=std::move(item); 
        ++size_; 
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
            ++size_;
            return;
            } 
        } else {
            size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
            Reserve(new_capacity);
            capacity_ = new_capacity;
        }
        std::move_backward(elements_.Get() + index, elements_.Get() + size_, elements_.Get() + size_ + 1);
        ++size_;
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