#include <matrix/vector.hh>

namespace matrix
{
    template <typename TYPE, int ROW>
    Vector<TYPE, ROW>::Vector() : super()
    {}

    template <typename TYPE, int ROW>
    inline const TYPE& Vector<TYPE, ROW>::operator[](int idx) const
    {
        return super::mat_[idx][0];
    }

    template <typename TYPE, int ROW>
    inline TYPE& Vector<TYPE, ROW>::operator[](int idx)
    {
        return super::mat_[idx][0];
    }
} // namespace matrix
