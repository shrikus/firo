#include "../RangeProver.h"
#include "../RangeVerifier.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(lelantus_range_proof_tests)

BOOST_AUTO_TEST_CASE(prove_verify)
{
    uint64_t n = 64;
    uint64_t m = 4;
    secp_primitives::GroupElement g_gen, h_gen1, h_gen2;
    g_gen.randomize();
    h_gen1.randomize();
    h_gen2.randomize();
    //creating generators g, h vectors
    std::vector <secp_primitives::GroupElement> g_;
    std::vector <secp_primitives::GroupElement> h_;
    for (int i = 0; i < n * m; ++i) {
        secp_primitives::GroupElement g;
        secp_primitives::GroupElement h;
        g.randomize();
        g_.push_back(g);
        h.randomize();
        h_.push_back(h);
    }

    std::vector<secp_primitives::Scalar> v_s, serials, randoms;
    std::vector<secp_primitives::GroupElement> V;
    for(int j = 0; j < m; ++j){
        secp_primitives::Scalar v(uint64_t(701+j)), random, serial;
        random.randomize();
        serial.randomize();
        v_s.push_back(v);
        randoms.push_back(random);
        serials.push_back(serial);
        V.push_back(g_gen * v +  h_gen1 * random + h_gen2 * serial);
    }

    lelantus::RangeProver<secp_primitives::Scalar, secp_primitives::GroupElement> rangeProver(g_gen, h_gen1, h_gen2, g_, h_, n);
    lelantus::RangeProof<secp_primitives::Scalar, secp_primitives::GroupElement> proof;
    rangeProver.batch_proof(v_s, serials, randoms, proof);

    lelantus::RangeVerifier<secp_primitives::Scalar, secp_primitives::GroupElement> rangeVerifier(g_gen, h_gen1, h_gen2, g_, h_, n);
    BOOST_CHECK(rangeVerifier.verify_batch(V, proof));
}

BOOST_AUTO_TEST_CASE(out_of_range_notVerify)
{
    uint64_t n = 4;
    uint64_t m = 4;
    secp_primitives::GroupElement g_gen, h_gen1, h_gen2;
    g_gen.randomize();
    h_gen1.randomize();
    h_gen2.randomize();
    //creating generators g, h vectors
    std::vector <secp_primitives::GroupElement> g_;
    std::vector <secp_primitives::GroupElement> h_;
    for (int i = 0; i < n * m; ++i) {
        secp_primitives::GroupElement g;
        secp_primitives::GroupElement h;
        g.randomize();
        g_.push_back(g);
        h.randomize();
        h_.push_back(h);
    }

    std::vector<secp_primitives::Scalar> v_s, serials, randoms;
    std::vector<secp_primitives::GroupElement> V;
    for(int j = 0; j < m; ++j){
        secp_primitives::Scalar v(uint64_t(17+j)), random, serial;
        random.randomize();
        serial.randomize();
        v_s.push_back(v);
        randoms.push_back(random);
        serials.push_back(serial);
        V.push_back(g_gen * v +  h_gen1 * random + h_gen2 * serial);
    }

    lelantus::RangeProver<secp_primitives::Scalar, secp_primitives::GroupElement> rangeProver(g_gen, h_gen1, h_gen2, g_, h_, n);
    lelantus::RangeProof<secp_primitives::Scalar, secp_primitives::GroupElement> proof;
    rangeProver.batch_proof(v_s, serials, randoms, proof);

    lelantus::RangeVerifier<secp_primitives::Scalar, secp_primitives::GroupElement> rangeVerifier(g_gen, h_gen1, h_gen2, g_, h_, n);
    BOOST_CHECK(!rangeVerifier.verify_batch(V, proof));
}

BOOST_AUTO_TEST_SUITE_END()