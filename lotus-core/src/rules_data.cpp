#include "lotus/input_method.hpp"
#include <map>
#include <string>

namespace lotus {

const std::map<std::u16string, InputMethodDefinition> INPUT_METHOD_DEFINITIONS = {
    { u"Telex", {
        { u"z", u"XoaDauThanh" },
        { u"s", u"DauSac" },
        { u"f", u"DauHuyen" },
        { u"r", u"DauHoi" },
        { u"x", u"DauNga" },
        { u"j", u"DauNang" },
        { u"a", u"A_Â" },
        { u"e", u"E_Ê" },
        { u"o", u"O_Ô" },
        { u"w", u"UOA_ƯƠĂ" },
        { u"d", u"D_Đ" }
    }},
    { u"VNI", {
        { u"0", u"XoaDauThanh" },
        { u"1", u"DauSac" },
        { u"2", u"DauHuyen" },
        { u"3", u"DauHoi" },
        { u"4", u"DauNga" },
        { u"5", u"DauNang" },
        { u"6", u"AEO_ÂÊÔ" },
        { u"7", u"UO_ƯƠ" },
        { u"8", u"A_Ă" },
        { u"9", u"D_Đ" }
    }},
    { u"VIQR", {
        { u"0", u"XoaDauThanh" },
        { u"'", u"DauSac" },
        { u"`", u"DauHuyen" },
        { u"?", u"DauHoi" },
        { u"~", u"DauNga" },
        { u".", u"DauNang" },
        { u"^", u"AEO_ÂÊÔ" },
        { u"+", u"UO_ƯƠ" },
        { u"*", u"UO_ƯƠ" },
        { u"(", u"A_Ă" },
        { u"d", u"D_Đ" }
    }},
    { u"Telex 2", {
        { u"z", u"XoaDauThanh" },
        { u"s", u"DauSac" },
        { u"f", u"DauHuyen" },
        { u"r", u"DauHoi" },
        { u"x", u"DauNga" },
        { u"j", u"DauNang" },
        { u"a", u"A_Â" },
        { u"e", u"E_Ê" },
        { u"o", u"O_Ô" },
        { u"w", u"UOA_ƯƠĂ__Ư" },
        { u"d", u"D_Đ" },
        { u"]", u"__ư" },
        { u"[", u"__ơ" },
        { u"}", u"_Ư" },
        { u"{", u"_Ơ" }
    }},
    { u"Telex W", {
        { u"z", u"XoaDauThanh" },
        { u"s", u"DauSac" },
        { u"f", u"DauHuyen" },
        { u"r", u"DauHoi" },
        { u"x", u"DauNga" },
        { u"j", u"DauNang" },
        { u"a", u"A_Â" },
        { u"e", u"E_Ê" },
        { u"o", u"O_Ô" },
        { u"w", u"UOA_ƯƠĂ__Ư" },
        { u"d", u"D_Đ" }
    }},
    { u"Telex + VNI", {
        { u"z", u"XoaDauThanh" }, { u"s", u"DauSac" }, { u"f", u"DauHuyen" }, { u"r", u"DauHoi" }, { u"x", u"DauNga" }, { u"j", u"DauNang" },
        { u"a", u"A_Â" }, { u"e", u"E_Ê" }, { u"o", u"O_Ô" }, { u"w", u"UOA_ƯƠĂ" }, { u"d", u"D_Đ" },
        { u"0", u"XoaDauThanh" }, { u"1", u"DauSac" }, { u"2", u"DauHuyen" }, { u"3", u"DauHoi" }, { u"4", u"DauNga" }, { u"5", u"DauNang" },
        { u"6", u"AEO_ÂÊÔ" }, { u"7", u"UO_ƯƠ" }, { u"8", u"A_Ă" }, { u"9", u"D_Đ" }
    }},
    { u"Telex + VNI + VIQR", {
        { u"z", u"XoaDauThanh" }, { u"s", u"DauSac" }, { u"f", u"DauHuyen" }, { u"r", u"DauHoi" }, { u"x", u"DauNga" }, { u"j", u"DauNang" },
        { u"a", u"A_Â" }, { u"e", u"E_Ê" }, { u"o", u"O_Ô" }, { u"w", u"UOA_ƯƠĂ" }, { u"d", u"D_Đ" },
        { u"0", u"XoaDauThanh" }, { u"1", u"DauSac" }, { u"2", u"DauHuyen" }, { u"3", u"DauHoi" }, { u"4", u"DauNga" }, { u"5", u"DauNang" },
        { u"6", u"AEO_ÂÊÔ" }, { u"7", u"UO_ƯƠ" }, { u"8", u"A_Ă" }, { u"9", u"D_Đ" },
        { u"'", u"DauSac" }, { u"`", u"DauHuyen" }, { u"?", u"DauHoi" }, { u"~", u"DauNga" }, { u".", u"DauNang" },
        { u"^", u"AEO_ÂÊÔ" }, { u"+", u"UO_ƯƠ" }, { u"*", u"UO_ƯƠ" }, { u"(", u"A_Ă" }, { u"\\", u"D_Đ" }
    }},
    { u"Microsoft layout", {
        { u"8", u"DauSac" }, { u"5", u"DauHuyen" }, { u"6", u"DauHoi" }, { u"7", u"DauNga" }, { u"9", u"DauNang" },
        { u"1", u"__ă" }, { u"!", u"_Ă" }, { u"2", u"__â" }, { u"@", u"_Â" }, { u"3", u"__ê" }, { u"#", u"_Ê" },
        { u"4", u"__ô" }, { u"$", u"_Ô" }, { u"0", u"__đ" }, { u")", u"_Đ" }, { u"[", u"__ư" }, { u"{", u"_Ư" }, { u"]", u"__ơ" }, { u"}", u"_Ơ" }
    }},
    { u"VNI Bàn phím tiếng Pháp", {
        { u"&", u"XoaDauThanh" }, { u"é", u"DauSac" }, { u"\"", u"DauHuyen" }, { u"'", u"DauHoi" }, { u"(", u"DauNga" },
        { u"-", u"DauNang" }, { u"è", u"AEO_ÂÊÔ" }, { u"_", u"UO_ƯƠ" }, { u"ç", u"A_Ă" }, { u"à", u"D_Đ" }
    }}
};

} // namespace lotus
