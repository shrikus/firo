#include "../../crypto/sha256.h"

namespace lelantus {

template<class Exponent, class GroupElement>
void LelantusPrimitives<Exponent, GroupElement>::commit(const GroupElement& g,
                                                        const std::vector<GroupElement>& h,
                                                        const std::vector<Exponent>& exp,
                                                        const Exponent& r,
                                                        GroupElement& result_out) {
    secp_primitives::MultiExponent mult(h, exp);
    result_out += g * r + mult.get_multiple();
}

template<class Exponent, class GroupElement>
GroupElement LelantusPrimitives<Exponent, GroupElement>::commit(
        const GroupElement& g,
        const Exponent& m,
        const GroupElement& h,
        const Exponent& r) {
    return g * m + h * r;
}

template<class Exponent, class GroupElement>
GroupElement LelantusPrimitives<Exponent, GroupElement>::double_commit(
        const GroupElement& g,
        const Exponent& m,
        const GroupElement& hV,
        const Exponent& v,
        const GroupElement& hR,
        const Exponent& r) {
    return g * m + hV * v + hR * r;
}

template<class Exponent, class GroupElement>
void LelantusPrimitives<Exponent, GroupElement>::convert_to_sigma(
        uint64_t num,
        uint64_t n,
        uint64_t m,
        std::vector<Exponent>& out) {
    int rem, nalNumber = 0;
    int j = 0;

    for (; num != 0; num /= n, j++)
    {
        rem = num % n;
        for (int i = 0; i < n; ++i)
        {
            if(i == rem)
                out.push_back(Exponent(uint64_t(1)));
            else
                out.push_back(Exponent(uint64_t(0)));
        }
    }

    for (; j < m; j++)
    {
        out.push_back(Exponent(uint64_t(1)));
        for (int i = 1; i < n; ++i)
        {
            out.push_back(Exponent(uint64_t(0)));
        }
    }
}

template<class Exponent, class GroupElement>
std::vector<uint64_t> LelantusPrimitives<Exponent, GroupElement>::convert_to_nal(
        uint64_t num,
        uint64_t n,
        uint64_t m) {
    std::vector<uint64_t> result;
    uint64_t rem, nalNumber = 0;
    uint64_t j = 0;
    for (; num != 0; num /= n, j++)
    {
        rem = num % n;
        result.push_back(rem);;
    }
    result.resize(m);
    return result;
}

template<class Exponent, class GroupElement>
void LelantusPrimitives<Exponent, GroupElement>::get_x(
        const GroupElement& A,
        const GroupElement& C,
        const GroupElement& D,
        Exponent& result_out) {
    CSHA256 hash;
    unsigned char data[3 * A.serialize_size];
    unsigned char* current = A.serialize(data);
    current = C.serialize(current);
    D.serialize(current);
    hash.Write(data, 3 * A.memoryRequired());
    unsigned char result_data[CSHA256::OUTPUT_SIZE];
    hash.Finalize(result_data);
    result_out = result_data;

}

template<class Exponent, class GroupElement>
void  LelantusPrimitives<Exponent, GroupElement>::get_x(
        const std::vector<SigmaPlusProof<Exponent, GroupElement>>& proofs,
        Exponent& result_out) {
    if (proofs.size() > 0) {
        CSHA256 hash;
        unsigned char data[3 * proofs.size() * 34];
        unsigned char* current = data;
        for (int i = 0; i < proofs.size(); ++i) {
            current = proofs[i].A_.serialize(current);
            current = proofs[i].C_.serialize(current);
            current = proofs[i].D_.serialize(current);
        }
        hash.Write(data, 3 * proofs.size() * 34);
        unsigned char result_data[32];
        hash.Finalize(result_data);
        result_out = result_data;
    }
    else
        result_out = uint64_t(1);
}

template<class Exponent, class GroupElement>
void LelantusPrimitives<Exponent, GroupElement>::new_factor(
        Exponent x,
        Exponent a,
        std::vector<Exponent>& coefficients) {
    std::vector<Exponent> temp;
    temp.resize(coefficients.size() + 1);
    for (int j = 0; j < coefficients.size(); j++)
        temp[j] += x * coefficients[j];
    for (int j = 0; j < coefficients.size(); j++)
        temp[j + 1] += a * coefficients[j];
    coefficients = temp;
}

template<class Exponent, class GroupElement>
void LelantusPrimitives<Exponent, GroupElement>::commit(
        const GroupElement& h,
        const Exponent& h_exp,
        const std::vector<GroupElement>& g_,
        const std::vector<Exponent>& L,
        const std::vector<GroupElement>& h_,
        const std::vector<Exponent>& R,
        GroupElement& result_out) {
    secp_primitives::MultiExponent g_mult(g_, L);
    secp_primitives::MultiExponent h_mult(h_, R);
    result_out += h * h_exp + g_mult.get_multiple() + h_mult.get_multiple();
}

template<class Exponent, class GroupElement>
void LelantusPrimitives<Exponent, GroupElement>::get_c(const GroupElement& u, Exponent& result) {
    CSHA256 hash;
    unsigned char data[34];
    u.serialize(data);
    hash.Write(&data[0], 34);
    unsigned char result_data[32];
    hash.Finalize(result_data);
    result = result_data;
}

template <class Exponent, class GroupElement>
void LelantusPrimitives<Exponent, GroupElement>::get_x(
        const GroupElement& L,
        const GroupElement& R,
        Exponent& result) {
    CSHA256 hash;
    unsigned char data[2 * L.memoryRequired()];
    L.serialize(data);
    R.serialize(data + 34);
    hash.Write(&data[0], 2 * 34);
    unsigned char result_data[32];
    hash.Finalize(result_data);
    result = result_data;
}

template <class Exponent, class GroupElement>
void LelantusPrimitives<Exponent, GroupElement>::get_x(const GroupElement& P, Exponent& result) {
    CSHA256 hash;
    unsigned char data[P.memoryRequired()];
    P.serialize(data);
    hash.Write(&data[0],34);
    unsigned char result_data[32];
    hash.Finalize(result_data);
    result = result_data;
}

template <class Exponent, class GroupElement>
Exponent LelantusPrimitives<Exponent, GroupElement>::scalar_dot_product(
        typename std::vector<Exponent>::const_iterator a_start,
        typename std::vector<Exponent>::const_iterator a_end,
        typename std::vector<Exponent>::const_iterator b_start,
        typename std::vector<Exponent>::const_iterator b_end) {
    Exponent result(uint64_t(0));
    auto itr_a = a_start;
    auto itr_b = b_start;
    while (itr_a != a_end || itr_b != b_end)
    {
        result += ((*itr_a) * (*itr_b));
        ++itr_a;
        ++itr_b;
    }
    return result;
}


template <class Exponent, class GroupElement>
void LelantusPrimitives<Exponent, GroupElement>::g_prime(
        const std::vector<GroupElement>& g_,
        const Exponent& x,
        std::vector<GroupElement>& result){
    Exponent x_inverse = x.inverse();
    result.reserve(g_.size() / 2);
    for (int i = 0; i < g_.size() / 2; ++i)
    {
        result.push_back(((g_[i] * x_inverse) + (g_[g_.size() / 2 + i] * x)));
    }
}

template <class Exponent, class GroupElement>
void LelantusPrimitives<Exponent, GroupElement>::h_prime(
        const std::vector<GroupElement>& h_,
        const Exponent& x,
        std::vector<GroupElement>& result) {
    Exponent x_inverse = x.inverse();
    result.reserve(h_.size() / 2);
    for (int i = 0; i < h_.size() / 2; ++i)
    {
        result.push_back(((h_[i] * x) + (h_[h_.size() / 2 + i] * x_inverse)));
    }
}

template <class Exponent, class GroupElement>
GroupElement LelantusPrimitives<Exponent, GroupElement>::p_prime(
        const GroupElement& P_,
        const GroupElement& L,
        const GroupElement& R,
        const Exponent& x){
    Exponent x_square = x.square();
    return L * x_square + P_ + R * (x_square.inverse());
}

template <class Exponent, class GroupElement>
Exponent LelantusPrimitives<Exponent, GroupElement>::delta(const Exponent& y, const Exponent& z, uint64_t n,  uint64_t m){
    Exponent y_;
    Exponent two;
    Exponent y_n(uint64_t(1));
    Exponent two_n(uint64_t(1));
    Exponent z_j =  z.exponent(uint64_t(3));
    Exponent z_sum(uint64_t(0));

    for(int j = 1; j <= m; ++j)
    {
        for(int i = 0; i < n; ++i)
        {
            y_ += y_n;
            y_n *= y;
        }
        z_sum += z_j;
        z_j *= z;
    }

    for(int i = 0; i < n; ++i)
    {
        two += two_n;
        two_n *= uint64_t(2);
    }

    return (z - z.square()) * y_ - z_sum * two;
}

}//namespace lelantus