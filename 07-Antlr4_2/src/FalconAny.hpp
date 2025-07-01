#pragma once

#include <antlr4-runtime.h>

#define BINARY_OPERATOR_OVERLOAD(op)                                           \
    const FalconVariable operator op(const FalconVariable &right) const        \
    {                                                                          \
        return FalconVariable(                                                 \
            static_cast<int32_t>(this->as<int32_t>() op right.as<int32_t>())); \
    }

#define ASSIGN_OPERATOR_OVERLOAD(op)                \
    Any &operator op(const FalconVariable & right)  \
    {                                               \
        this->as<int32_t>() op right.as<int32_t>(); \
        return *this;                               \
    }

#define UNARY_OPERATOR_OVERLOAD(op)                          \
    Any operator op() const                                  \
    {                                                        \
        return static_cast<int32_t>(op this->as<int32_t>()); \
    }

/**
 * 基础数据类型
 */
enum class FalconType
{
    Integer,  // int32_t
};

class FalconVariable : public antlrcpp::Any
{
  public:
    FalconVariable() : Any(), type_(FalconType::Integer)
    {
    }

    FalconVariable(Any &that) : Any(that), type_(FalconType::Integer)
    {
        setType(that);
    }

    FalconVariable(Any &&that)
        : Any(std::move(that)), type_(FalconType::Integer)
    {
        setType(that);
    }

    FalconVariable(const Any &that) : Any(that), type_(FalconType::Integer)
    {
        setType(that);
    }

    FalconVariable(const Any &&that)
        : Any(std::move(that)), type_(FalconType::Integer)
    {
        setType(that);
    }

    operator bool() const
    {
        return this->as<int32_t>() != 0;
    }

  public:
    /**
     * 运算符重载们 @{
     *
     * 单目运算符 @{
     */
    BINARY_OPERATOR_OVERLOAD(+)
    BINARY_OPERATOR_OVERLOAD(-)
    BINARY_OPERATOR_OVERLOAD(*)
    BINARY_OPERATOR_OVERLOAD(/)
    BINARY_OPERATOR_OVERLOAD(%)
    BINARY_OPERATOR_OVERLOAD(<<)
    BINARY_OPERATOR_OVERLOAD(>>)
    BINARY_OPERATOR_OVERLOAD(==)
    BINARY_OPERATOR_OVERLOAD(!=)
    BINARY_OPERATOR_OVERLOAD(>)
    BINARY_OPERATOR_OVERLOAD(<)
    BINARY_OPERATOR_OVERLOAD(>=)
    BINARY_OPERATOR_OVERLOAD(<=)
    BINARY_OPERATOR_OVERLOAD(&)
    BINARY_OPERATOR_OVERLOAD(|)
    BINARY_OPERATOR_OVERLOAD(^)
    BINARY_OPERATOR_OVERLOAD(&&)
    BINARY_OPERATOR_OVERLOAD(||)
    /**
     * @}
     */

    /**
     * 赋值运算符 @{
     */
    ASSIGN_OPERATOR_OVERLOAD(=)
    ASSIGN_OPERATOR_OVERLOAD(+=)
    ASSIGN_OPERATOR_OVERLOAD(-=)
    ASSIGN_OPERATOR_OVERLOAD(*=)
    ASSIGN_OPERATOR_OVERLOAD(/=)
    ASSIGN_OPERATOR_OVERLOAD(%=)
    ASSIGN_OPERATOR_OVERLOAD(<<=)
    ASSIGN_OPERATOR_OVERLOAD(>>=)
    ASSIGN_OPERATOR_OVERLOAD(&=)
    ASSIGN_OPERATOR_OVERLOAD(|=)
    ASSIGN_OPERATOR_OVERLOAD(^=)

    /**
     * @}
     */

    /**
     * 自增自减运算符 @{
     */
    Any &operator++()
    {
        this->as<int32_t>() = this->as<int32_t>() + 1;
        return *this;
    }

    Any operator++(int)
    {
        auto value = this->as<int32_t>();
        this->as<int32_t>() = value + 1;
        return value;
    }

    Any &operator--()
    {
        this->as<int32_t>() = this->as<int32_t>() - 1;
        return *this;
    }

    Any operator--(int)
    {
        auto value = this->as<int32_t>();
        this->as<int32_t>() = value - 1;
        return value;
    }
    /**
     * @}
     */

    /**
     * 前置单目运算符 @{
     */
    UNARY_OPERATOR_OVERLOAD(+)
    UNARY_OPERATOR_OVERLOAD(-)
    UNARY_OPERATOR_OVERLOAD(~)
    UNARY_OPERATOR_OVERLOAD(!)
    /**
     * @}
     *
     * @}
     */

  private:
    void setType(const Any &)
    {
        type_ = FalconType::Integer;
    }

  private:
    FalconType type_;
};

#undef BINARY_OPERATOR_OVERLOAD
#undef ASSIGN_OPERATOR_OVERLOAD
#undef UNARY_OPERATOR_OVERLOAD
