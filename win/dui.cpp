#include "wochat.h"

#include "bitcoin/src/crypto/aes.h"
#include "bitcoin/src/crypto/sha256.h"
#include "bitcoin/src/crypto/chacha20.h"
#include "bitcoin/src/util/strencodings.h"
#include "chromium/ui/gfx/geometry/rect.h"
#include "chromium/ui/gfx/geometry/point.h"
#include "chromium/base/time/default_clock.h"
#include "chromium/base/memory/page_size.h"
#include "chromium/base/enterprise_util.h"
#include "chromium/base/check_op.h"
#include "chromium/ui/base/hit_test.h"
#include "chromium/ui/base/fullscreen_win.h"
#include "chromium/ui/gfx/scrollbar_size.h"
#include "chromium/ui/gfx/favicon_size.h"

#define BOOST_CHECK(expr) assert(expr)
#define BOOST_CHECK_EQUAL(v1, v2) assert((v1) == (v2))
#define BOOST_CHECK_THROW(stmt, excMatch) { \
        try { \
            (stmt); \
            assert(0 && "No exception caught"); \
        } catch (excMatch & e) { \
	} catch (...) { \
	    assert(0 && "Wrong exception caught"); \
	} \
    }
#define BOOST_CHECK_NO_THROW(stmt) { \
        try { \
            (stmt); \
	} catch (...) { \
	    assert(0); \
	} \
    }

static void TestChaCha20(const std::string& hex_message, const std::string& hexkey, ChaCha20::Nonce96 nonce, uint32_t seek, const std::string& hexout)
{
    auto key = ParseHex<std::byte>(hexkey);
    assert(key.size() == 32);
    auto m = ParseHex<std::byte>(hex_message);
    ChaCha20 rng{ key };
    rng.Seek(nonce, seek);
    std::vector<std::byte> outres;
    outres.resize(hexout.size() / 2);
    assert(hex_message.empty() || m.size() * 2 == hexout.size());

    // perform the ChaCha20 round(s), if message is provided it will output the encrypted ciphertext otherwise the keystream
    if (!hex_message.empty()) {
        rng.Crypt(m, outres);
    }
    else {
        rng.Keystream(outres);
    }
    BOOST_CHECK_EQUAL(hexout, HexStr(outres));
    if (!hex_message.empty()) {
        // Manually XOR with the keystream and compare the output
        rng.Seek(nonce, seek);
        std::vector<std::byte> only_keystream(outres.size());
        rng.Keystream(only_keystream);
        for (size_t i = 0; i != m.size(); i++) {
            outres[i] = m[i] ^ only_keystream[i];
        }
        BOOST_CHECK_EQUAL(hexout, HexStr(outres));
    }

#if 0
    // Repeat 10x, but fragmented into 3 chunks, to exercise the ChaCha20 class's caching.
    for (int i = 0; i < 10; ++i) {
        size_t lens[3];
        lens[0] = InsecureRandRange(hexout.size() / 2U + 1U);
        lens[1] = InsecureRandRange(hexout.size() / 2U + 1U - lens[0]);
        lens[2] = hexout.size() / 2U - lens[0] - lens[1];

        rng.Seek(nonce, seek);
        outres.assign(hexout.size() / 2U, {});
        size_t pos = 0;
        for (int j = 0; j < 3; ++j) {
            if (!hex_message.empty()) {
                rng.Crypt(Span{ m }.subspan(pos, lens[j]), Span{ outres }.subspan(pos, lens[j]));
            }
            else {
                rng.Keystream(Span{ outres }.subspan(pos, lens[j]));
            }
            pos += lens[j];
        }
        BOOST_CHECK_EQUAL(hexout, HexStr(outres));
    }
#endif
}

unsigned char inputdata[8] = { 'x', 'B', 'C', 'D', 'E', 'F', 'G', 0x00 };

int bitcointest();

int dummy()
{
#if 0
    int i;
    unsigned char hash[32] = { 0 };

    unsigned char plantext[32] = {
    0x31, 0x5F, 0x5B, 0xDB, 0x76, 0xD0, 0x78, 0xC4,
    0x3B, 0x8A, 0xC0, 0x06, 0x4E, 0x4A, 0x01, 0x64,
    0x61, 0x2B, 0x1F, 0xCE, 0x77, 0xC8, 0x69, 0x34,
    0x5B, 0xFC, 0x94, 0xC7, 0x58, 0x94, 0xED, 0xD3,
    };
    unsigned char ciphertext[32] = { 0 };
    unsigned char plantext2[32] = { 0 };
    unsigned char K[32] = {
        0xF3,0xBB,0x6F,0x11,0x7C,0xC5,0x45,0xF8,
        0x53,0x0B,0x8F,0x3E,0x2D,0xEF,0xA4,0xD0,
        0xF9,0x46,0x3A,0x38,0x45,0x2C,0x04,0x1D,
        0x2C,0xA6,0x4D,0x39,0x91,0x03,0xB6,0xDE
    };
    unsigned char IV[16] = {
        0xDB,0xD5,0x10,0x1D,0x05,0xA4,0xA4,0x65,
        0x77,0xAF,0xC3,0x92,0x54,0xBB,0x9C,0xCB 
    };

    {
        AES256CBCEncrypt e(K, IV, false);
        e.Encrypt(plantext, 32, ciphertext);
    }
    {
        AES256CBCDecrypt d(K, IV, false);
        d.Decrypt(ciphertext, 32, plantext2);
    }

    i = memcmp(plantext, plantext2, 32);

    CSHA256 s;
    s.Write(inputdata, 7);
    s.Finalize(hash);


    /* Example from RFC8439 section 2.4.2. */
    TestChaCha20("4c616469657320616e642047656e746c656d656e206f662074686520636c6173"
        "73206f66202739393a204966204920636f756c64206f6666657220796f75206f"
        "6e6c79206f6e652074697020666f7220746865206675747572652c2073756e73"
        "637265656e20776f756c642062652069742e",
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f",
        { 0, 0x4a000000 }, 1,
        "6e2e359a2568f98041ba0728dd0d6981e97e7aec1d4360c20a27afccfd9fae0b"
        "f91b65c5524733ab8f593dabcd62b3571639d624e65152ab8f530c359f0861d8"
        "07ca0dbf500d6a6156a38e088a22b65e52bc514d16ccf806818ce91ab7793736"
        "5af90bbf74a35be6b40b8eedf2785e42874d");

   //bitcointest();

    RECT rc;
    rc.left = rc.top = 10;
    rc.right = 101; rc.bottom = 102;

    //gfx::Point p(0x11223344);
    //r.AdjustForSaturatedRight(10);
    //base::DefaultClock ck;
    //ck.Now();

    size_t st = base::GetPageSize();

    //bool bret = base::IsManagedOrEnterpriseDevice();
    char* str = logging::CheckOpValueStr(112);

    //bool bRet = ui::IsPlatformFullScreenMode();
    int w = gfx::scrollbar_size();
    int h = w;
    gfx::CalculateFaviconTargetSize(&w, &h);

	return i;
#endif
    return 0;
}