#ifndef QUEUE_H
#define QUEUE_H

#include <memory>
#include <cstdint>
#include "Allocator.h"

template <class T, class Allocator = std::allocator<T>>
class TQueue
{
private:
    struct TNode;
    class TIterator;

public:
    using value_type = T;
	using allocator_type = Allocator;
	using size_type = uint64_t;
	using reference = value_type &;
	using const_reference = const value_type &;
	using pointer = typename std::allocator_traits<Allocator>::pointer;
	using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
	using iterator = TIterator;
	using const_iterator = const TIterator;

    TQueue() : Size_(0), Allocator_()
    {
        fakeElement = NodeAllocate(value_type());
    }

    TQueue(const TQueue &other)
    {
        Allocator_ = std::allocator_traits<TAllocatorNode>::select_on_container_copy_construction(other.Allocator_);
        Size_ = other.Size_;

        fakeElement = NodeAllocate(other.fakeElement->Value);
        std::shared_ptr<TNode> itNew = fakeElement;
        std::shared_ptr<TNode> itOld = other.fakeElement;
        for (int i = 0; i < Size_; ++i, itNew = itNew->Next, itNew = itOld->Next)
        {
            itNew->Next = NodeAllocate(itOld->Next->Value);
            itNew->Next->Prev = itNew;
        }
        if (Size_ >= 2)
        {
            itNew->Next = fakeElement;
            fakeElement->Prev = itNew;
        }
    }

    TQueue &operator=(const TQueue &other)
    {
        Allocator_ = std::allocator_traits<TAllocatorNode>::select_on_container_copy_construction(other.Allocator_);
        Size_ = other.Size_;

        fakeElement->Next = nullptr;
        fakeElement = NodeAllocate(other.fakeElement->Value);
        std::shared_ptr<TNode> itNew = fakeElement;
        std::shared_ptr<TNode> itOld = other.fakeElement;
        for (int i = 0; i < Size_; ++i, itNew = itNew->Next, itNew = itOld->Next)
        {
            itNew->Next = NodeAllocate(itOld->Next->Value);
            itNew->Next->Prev = itNew;
        }
        if (Size_ >= 2)
        {
            itNew->Next = fakeElement;
            fakeElement->Prev = itNew;
        }
        return *this;
    }

    ~TQueue()
    {
        fakeElement->Next = nullptr;
        fakeElement = nullptr;
    }

    void Push(const value_type &value)
    {
        InsertNonEmpty(fakeElement, value);
    }

    void Pop()
    {
        if (Size_ == 0)
            throw std::logic_error("REMOVE_ERROR: List is empty");
        Remove(fakeElement->Next);
    }

    const value_type &Front() const
    {
        if (Size_ == 0)
            throw std::logic_error("ACCESS_ERROR: List is empty");
        return fakeElement->Next->Value;
    }

    const value_type &Back() const
    {
        if (Size_ == 0)
            throw std::logic_error("ACCESS_ERROR: List is empty");
        return fakeElement->Prev.lock()->Value;
    }

    value_type &Front()
    {
        return (this)->Front();
    }

    value_type &Back()
    {
        return (this)->Back();
    }

    const value_type &Top() const
    {
        return Back();
    }

    value_type &Top()
    {
        return Back();
    }

    size_type Size()
    {
        return Size_;
    }

    bool Empty()
    {
        return Size_ == 0;
    }

    const_iterator begin() const
    {
        if (Size_ == 0)
            return TIterator(fakeElement, this);
        return TIterator(fakeElement->Next, this);
    }

    const_iterator end() const
    {
        return TIterator(fakeElement, this);
    }

    iterator begin()
    {
        if (Size_ == 0)
            return TIterator(fakeElement, this);
        return TIterator(fakeElement->Next, this);
    }

    iterator end()
    {
        return TIterator(fakeElement, this);
    }

    iterator Insert(iterator pos, const value_type &value)
    {
        InsertNonEmpty(pos.Item_.lock(), value);
        return TIterator(pos.Item_.lock()->Prev.lock(), this);
    }

    iterator EraseByPos(iterator pos)
    {
        TIterator res = pos;
        ++res;
        Remove(pos.Item_.lock());
        return res;
    }

private:
    struct TNode
    {
        std::shared_ptr<TNode> Next;
        std::weak_ptr<TNode> Prev;
        value_type Value;

        TNode(const value_type &val) : Value(val), Next(nullptr)
        {
        }
    };

    class TIterator
    {
    public:
        using difference_type = uint32_t;
		using value_type = TQueue::value_type;
		using reference = TQueue::value_type&;
		using pointer = TQueue::value_type*;
		using iterator_category = std::bidirectional_iterator_tag;

        TIterator(std::shared_ptr<TNode> item, TQueue const *lst) : Item_(item), Lst_(lst)
        {
        }

        ~TIterator() = default;

        TIterator(const TIterator &other)
        {
            Item_ = other.Item_;
            Lst_ = other.Lst_;
        }

        TIterator &operator=(const TIterator &other)
        {
            Item_ = other.Item_;
            Lst_ = other.Lst_;
            return *this;
        }

        TIterator &operator++()
        {
            if (Lst_->Size_ == 0)
                return *this;
            if (Lst_->Size_ == 1 && Item_.lock() == Lst_->fakeElement->Next)
            {
                Item_ = Lst_->fakeElement;
                return *this;
            }
            Item_ = Item_.lock()->Next;
            return *this;
        }

        const TIterator operator++(int)
        {
            if (Lst_->Size_ == 0)
                return *this;
            if (Lst_->Size_ == 1 && Item_.lock() == Lst_->fakeElement->Next)
            {
                Item_ = Lst_->fakeElement;
                return *this;
            }
            std::shared_ptr<TNode> res = Item_.lock();
            Item_ = Item_.lock()->Next;
            return TIterator(res);
        }

        TIterator &operator--()
        {
            if (Lst_->Size_ == 0)
                return *this;
            Item_ = Item_.lock()->Prev;
            return *this;
        }

        const TIterator operator--(int)
        {
            if (Lst_->Size_ == 0)
                return *this;
            std::shared_ptr<TNode> res = Item_.lock();
            Item_ = Item_.lock()->Prev;
            return TIterator(res);
        }

        value_type &operator*()
        {
            return Item_.lock()->Value;
        }

        pointer operator->()
        {
            return &Item_.lock()->Value;
        }

        bool operator!=(const TIterator &other) const noexcept
        {
            return Item_.lock() != other.Item_.lock();
        }

        bool operator==(const TIterator &other) const noexcept
        {
            return Item_.lock() == other.Item_.lock();
        }

    private:
        std::weak_ptr<TNode> Item_;
        TQueue const *Lst_;
        friend class TQueue;
    };

    std::shared_ptr<TNode> fakeElement;
    uint64_t Size_;
    using TAllocatorNode = typename allocator_type::template rebind<TNode>::other;
    TAllocatorNode Allocator_;

    std::shared_ptr<TNode> NodeAllocate(const value_type &value)
    {
        TNode *node = Allocator_.allocate(1);
        Allocator_.construct(node, value);

        auto deleter = [this](TNode *ptr) {
            this->Allocator_.destroy(ptr);
            this->Allocator_.deallocate(ptr, 1);
        };
        std::shared_ptr<TNode> ptr(node, deleter);
        return ptr;
    }

    void InsertEmpty(const value_type &value)
    {
        fakeElement->Next = NodeAllocate(value);
        fakeElement->Next->Prev = fakeElement;
        fakeElement->Prev = fakeElement->Next;
    }

    void InsertNonEmpty(std::shared_ptr<TNode> item, const value_type &value)
    {
        if (Size_ == 0)
        {
            InsertEmpty(value);
            ++Size_;
            return;
        }
        std::shared_ptr<TNode> newElem = NodeAllocate(value);
        newElem->Next = item;
        newElem->Prev = item->Prev;
        item->Prev.lock()->Next = newElem;
        item->Prev = newElem;
        ++Size_;
    }

    void Remove(std::shared_ptr<TNode> item)
    {
        if (Size_ == 1)
        {
            fakeElement->Next = nullptr;
        }
        else
        {
            if (Size_ == 2)
            {
                if (item->Next == fakeElement)
                {
                    fakeElement->Next->Next = nullptr;
                    fakeElement->Prev = fakeElement->Next;
                }
                else
                {
                    item->Next->Next = nullptr;
                    item->Next->Prev = fakeElement;
                    fakeElement->Next = item->Next;
                }
                --Size_;
                return;
            }
            item->Next->Prev = item->Prev;
            item->Prev.lock()->Next = item->Next;
        }
        --Size_;
    }
};

#endif
