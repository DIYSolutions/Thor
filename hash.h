#pragma once
#include "memory_basic.h"

/// hash
constexpr U64 PieceKeys[13][64] = {
{
0x9904662fb3c24ae1, 0xc59adcabb4a95f90, 0x9dbe36bc60dda6e9, 0xb1766ba9896d953c, 0x772183cf804c8124, 0xe881b24726834db7, 0x7a9bce6cd689074d, 0x8c8879aee145a6a6, 0xfba07e96bb3ec509, 0x8764a787f72e9ad4, 0xbd7f9fe5bfface45, 0xb442698375c4830a, 0xc17b75ab83990120, 0xa46a08bba5a05878, 0x6b9faf848d24df32, 0x6753de5f6fa48ddc, 0xd629f78526f94944, 0x26cc7341b35e6c4, 0xfd6f80c961d95a1, 0x27dec208acc8c09d, 0x1f316487f9ed58b0, 0xa6d32240bddcd772, 0xde093a4b2540187e, 0x5d132f3549f4c080, 0x267d48f264665753, 0xfb8cef3587dfaf14, 0x608fd08b6a6e368e, 0x6f307d7be32bac49, 0x15fffb1a5447ba61, 0xdfba2987af4ec89c, 0x6c2e5adc9973401d, 0x709fd3e4a50677, 0x231afafa281cd42c, 0x3d7963f247095f1e, 0x3f0e8927958611f4, 0x5d5a96a7e66a5fa4, 0x924fc00d43e7e732, 0x235e83b3236781d3, 0x745b8122be9c58f, 0x56fcc6766c8ea52a, 0x53b8127de215d078, 0x1810fed12c87765f, 0x56005b53fb0ba8d, 0xe18f7401460ab807, 0xb0c67ca0928f9d18, 0x6924676a8324d54, 0xe7763321689f6d69, 0x59ff1711a4353004, 0x6bce71c3b9ec9f16, 0xf9acf65a256a2cf7, 0xa14957b4272bcf68, 0x6cdf44c5a4ddef11, 0xd9d5d74902c45579, 0xe50a5236d3929643, 0x5de79b3d2fa293d3, 0x81450273a905e8f5, 0x572c0d16531ebb97, 0x72714a566f3b282d, 0xf585bf26e66a668, 0x4209221ab23cc325, 0x8f4c2250380a53b1, 0xb1ad50294e88a528, 0x18d195358754bf0b, 0x77f2f9632097b0f1}
,{
0x5883b366994a80c1, 0xb19c33f0373f3ee9, 0x87f34c29f24346c2, 0x5f4a942fcb4d2fe7, 0x9be47e74672a8390, 0x80e51474486cec6c, 0x198cd717b6a701e1, 0xb5393bf8dec8871, 0xf9fc4ae954712f0c, 0xbcd699d1e1b2ce38, 0xae68c4a26854b382, 0x922f5b5d843c36c2, 0x34d631898e6fa7da, 0x9ceda44f923164e0, 0x6ca257cd275fae39, 0x63a7fba24eaf1ff1, 0xdfc1c1b8c5364328, 0x1183c848390e9dcb, 0x22007dcfc5208607, 0x4561fd451982b765, 0xbd65111875e4d8c5, 0x74258023a1a37a36, 0x83db92bcd054578d, 0xe6f7dc7c8075f871, 0xfc941274091d9c75, 0x60118d5f8543d478, 0x3109b9f42a5f5882, 0xe87b75f089235841, 0xbc7180feb798527f, 0x15e11a9fcaa0786, 0x2252bf6aa7ada568, 0x35e176398dfe13f5, 0xa857dcdf46cff389, 0xa1483906cafefcb8, 0xfded06bd9d39007b, 0x41d32cf353e9ff0d, 0x408860e8fda13a6, 0x6e2a7e0ccc85e3cb, 0x1a821d179d2675ec, 0x3269945ce4733605, 0x48a63283a25520ad, 0xf59aa12c32e51927, 0xf63b12e7448a194d, 0x4a4f2e89e6acd942, 0xcaf38cb0757c4bcd, 0xcf30fc081911fe64, 0x8e54d261dc4ad04c, 0xcb492350a7342d, 0x9b5cf74f7958b260, 0xee7629e018d6c908, 0x1bbd37d9ef20dea5, 0x5333e8b92b8c749f, 0x584843803a1319fe, 0xca7a2e086c020b93, 0xf13660e3d6b9a753, 0x2486ca28457d67e, 0xa32df6356767d3d3, 0x1192b173832d7cbe, 0x5f84f74aa1fb5707, 0xd5f42c6f8332e7a6, 0x40f929f032349b32, 0x47c1204b1c26c101, 0xfc22ecd596636bdb, 0xd9f91fe9836c480b}
,{
0x863029ce1230836b, 0x8642409881fcba54, 0xefcecdc6e8bce48, 0x8e01b2a4a6148a1d, 0x676532e665b948db, 0x4ac38afe80a7a237, 0x83b6b8ac17caef57, 0x5b9bf4203936176d, 0xde6e98f67dc59943, 0x5cc23e823652f1f6, 0x92f7b3bdd0f5a015, 0x8c9c05786db03d8f, 0xece79b5ec7fa275b, 0x279158628c47dd27, 0xc03ef4b96c43964a, 0xb46655a6f5b0c740, 0x4c78c770e8d89da7, 0xf89490401776275, 0xf0c0604bd64f526a, 0xf6a20d5405cdc5c8, 0x1568750a7814448a, 0x60dd2ee790a3b6a1, 0x17c902cc64365acd, 0x4dd3db3792c0eea3, 0xcbf31a8430ffef9a, 0x3e99b0b3dc2b8dc7, 0xd2b2dafcade801c, 0x7c60620a05bfb15d, 0x9cebe4ee49248634, 0xe04c1f4f297286de, 0x2a330dd081591f0d, 0xba76d905017669bb, 0x8a25f19cfef13b51, 0xe4cfb90755ec3d84, 0x31a3ff80705f1350, 0xbb62dff5b1a1d221, 0x3f233605528c4898, 0xb8c596cb8fc5c5f9, 0xef3b956f30f84fca, 0xecc7b7db7f4a1a31, 0x2e8e92ebfac6c0b5, 0x47321c757c2f176a, 0x33f97603c3b5002, 0xba42e11ae39b45b1, 0x10fb7fdf482d4cea, 0x22c3d5209546a871, 0x86918dae0e2f56d9, 0xe7f5f03eb35fb7b0, 0x456e8c07483ae9e5, 0xce42149629a341da, 0x2bcb7ea05dd02e7f, 0xce797a475d1b0de9, 0x9c2e81409aaaf7d2, 0xc2a346c2c77bcba9, 0x69e472902c2ddda3, 0x3ab7298d73c43068, 0xdc5c268a8aeab1b2, 0x294ba642bd72985a, 0xc4dd23ddccae85eb, 0x5c2d3c31401d396c, 0x3cd30ec35fabdfeb, 0x78f342f7fb027268, 0x20f44fb568735fa4, 0x64179d1df2af998d}
,{
0x30d1f0b5b2955a21, 0x4bb531b53a827011, 0x14e30ca35ed706bb, 0x3003989830651c20, 0xf0ebb5758217ac45, 0x3cc52df5fed45159, 0xc7a1bbfa85130ced, 0x5249dd6013c22fd9, 0xb0bf1a0dcf5c72ed, 0x544f46a80eae6c4e, 0xae33fb0866fe053c, 0x62f0ab639cc097b0, 0xd9f7f17bba58c2e4, 0x5ffd3f04b783eb8b, 0x65fd89198af17ca3, 0x3b7720fd29486806, 0x2d1723fd2e023002, 0xdea59c78020f6301, 0x401f0750dac8140b, 0x2806e7c5b0497408, 0x308398ad4e9bc73e, 0x7da1ca9ca27dfef2, 0x6a598749b404af7e, 0x608f51f619768f55, 0x7e624672bc2b9002, 0x6e4d8e420e37513f, 0x9fc0d8e810e6e9b, 0xf4474957eba047dd, 0xa9b739391b0de8c0, 0x622c3087414900e5, 0x85579cde1d95e24a, 0x7a1860c62bacfc0a, 0xeb4d4882b4a152c4, 0x1437db03dd71dfc3, 0x103b6f3fe81833b2, 0xa071293e2632f122, 0x1ee78c41621e814a, 0x3dd7cdff6cc6aa9e, 0x97fa11e8427f3069, 0x4eff5b813db3cded, 0xf1392d06538a3fd1, 0xfdff09bcb2047768, 0x1375a8f023e2efec, 0x24d29744b7761d94, 0x97261c7da4c183af, 0x30f2b3d0507195bd, 0xd3b42fd63d6d064f, 0x156708f3a0febeca, 0x5ecb933bd2b2bce5, 0x9f38918c356f65af, 0xc97c5bd7945b7836, 0xbf17d700082b632f, 0x72d080da82aa3a36, 0xf22d80768d9e68c5, 0xac797512531ee405, 0x3d7947c39feb82f2, 0x107e8b662dc7b474, 0x84c445d3efac113f, 0x4868eb7bf3a11454, 0xe21be0ef8b9ce8b6, 0x69218b19e28e551b, 0x4eeb9e3f86bb939d, 0xbccddca4c4a46fa7, 0x4636718fab0139b8}
,{
0x4f30ff6dae984c22, 0xd21c8655a66ebe61, 0xb3adcf4f6c46795c, 0x7fb88799e0de1e2f, 0x27bfea726266490f, 0x3592056a52921c11, 0xebd5b9d2e9a0d7e5, 0x2c45020fee316df5, 0x80b5cf7f4154eb49, 0xb8a6335464656380, 0xcb25d851778d1439, 0x5c9540af9c0e0565, 0xe44ee22f4fabfdb3, 0x39d92245d7857a4e, 0x656647bd92878686, 0x2c262348e16a684, 0x7a64c2ad90d11178, 0x890045b468770daf, 0x3c242aae83aca429, 0xedf8d229b7950a65, 0xfffe9451e79b4c21, 0xb31bbf52ced1056a, 0x94150d04b2e022e0, 0x461237480d3706c7, 0x25a9eb8f6ac27cee, 0xe550b1a7d065522, 0x4848b5fe20c253f, 0x8198036a6d65ec01, 0xfd1b202f5f724d64, 0x84a67362efcd6fd9, 0x3d48136393836461, 0x78ae960c9941ff20, 0xc695e0e0ebfea123, 0x23a8860c772e61b3, 0x87bb41cc3d848ae1, 0x1f66925d783bb450, 0xa69db512b4b1f8fb, 0xd00933b8432a53f9, 0x2f4593538cdb350a, 0x79f859de404f8d79, 0xc46e0a2334c06d3f, 0x329b211de057f5f, 0x45e54d6698a1ca4b, 0x95660c96bcddc12b, 0x49bb98db055a935a, 0xd165ca271e325087, 0x8144f3aa092423ee, 0x87067fd20e240dbd, 0xe43bc03c64e162a1, 0x3f81cbd279dac2c7, 0x4d78f4f93e1350b, 0x2d772c739b5bdcb1, 0x1176629db0326c69, 0x35c10f33460ab528, 0xc57d7d3a4ead27b8, 0xf576e5d5516d1a5c, 0x355d4218201f7b59, 0x5724bd370b793daf, 0xdb3007f4bbf2a383, 0x7d28ba3a685065c4, 0xc42ce943fcfd6e02, 0xbb51e832baf23ee1, 0x9d3722745216b122, 0x683dcdcf0901fccb}
,{
0xd0155d45da58e0ae, 0x51a0ad8a6b634382, 0xfd6508b01ff7ef6a, 0x2f88513a4e1f888b, 0x1329e62c4cc57a77, 0x7bd2096bab82649f, 0xdedadb0490930d7f, 0xce75c4bf3922ae01, 0x461aea9b6bcff19a, 0x76eeb59691183dcd, 0xcd539696fc3be4b, 0x48f1c8ebd63806ee, 0x3c34cbca92129b0a, 0xb0cd7c360fecb3b0, 0xbe011375b2271932, 0x1c302ddde8bbe6fb, 0x4529bed26d669949, 0x20c1d7c8424e90bf, 0xb8d7d234e21c9c04, 0x14dfa2101a52111d, 0x7d215047d132fe74, 0x1f2a91a2d3ebc47, 0xa583b0cdb3e94234, 0xed4471bd86a24139, 0xcb91cf2a5ce2759d, 0x5d83c4caa388faf, 0x21cc88f422f96548, 0x1dbad7d19dafae09, 0x695fec212873475f, 0x5d62e5f1409f0dfc, 0x338c6830da425a92, 0xb2211168f6d5673f, 0xec7ea07092acdad, 0x1f49f13118c28196, 0x8e2bd2f549c4021e, 0x1eaad4e30e5db3e9, 0xf18dacc960642aeb, 0x9a01a806528ec44a, 0x77a62a806b2c3aec, 0x769abd8347be5515, 0x83f543920687d841, 0x87da0e8529d1b591, 0x81967a941797f060, 0xf656ca16e71d0b7, 0x3d03bb480417df2d, 0x23c51a34f2282310, 0x72cba3f9f07434f7, 0x42bc516a0f70a8c8, 0xfa8756596ae55258, 0xc545be7894862760, 0xc5e588d8bd829e3c, 0x29ff5831654ca2b0, 0x756816da0161d9ab, 0xa205b7091860c310, 0xb9788fd841f8d5fc, 0x8597f252b0e982e6, 0x58c057f0121065a2, 0x8b94a97ca27933ea, 0x763ae3186ac394b7, 0xabb77a1f8d8e0d7, 0x543d23911118dde1, 0xd7ce66e0b346ce73, 0xc7b8dff397ea7957, 0xda15926c0950f706}
,{
0xda46e18ea9f7df06, 0xca6966630effddb6, 0xcc10dc95e30cf225, 0x69daf7096ec99372, 0xb271ccf3a753dbbf, 0xce2d220a1943cd43, 0x9738d860a50aaaf9, 0x15c3f7fdc5368c3e, 0xf0b62cb186ecb51d, 0xa450ce7f0e662173, 0x4e4a1c205cc1b4b2, 0x4f6e37a8d5dedc8c, 0xb9f17c9d6bad9e29, 0xd881b6e5645901ef, 0x5755df1138efd9e7, 0x71a828889dd7cdaa, 0x862dc3bbbfe1deb5, 0xa01215c4bd35da72, 0xc241f5b4673754dc, 0xd1235d09771fd072, 0xa933c4def982c975, 0x82ced402756555cb, 0x772d3f746a3f5ab8, 0x4d0e37e6be586aec, 0x51e1e69392acf950, 0x63ff06e8f76f3727, 0x5edc181b18f5aff7, 0xba177e1d2f1ede36, 0x18cd9f5de830a9f1, 0xba099642df5e558d, 0x91ad021608f23a1b, 0x3a581b6a9107e8a6, 0xceab2345d044bfa2, 0xeb440381d7cebdab, 0x2194ee8b45f6c2a7, 0x9ca5fa5f6f38482f, 0xf2ff7fb51b57525b, 0x7e697af97a94bdd2, 0xaba3d83f1892df4f, 0x25ce60fff49fe102, 0x3396a2a39101d016, 0x5537e8269f09e03d, 0xd59116483de5336a, 0x7e38d2f4a6d2ac76, 0x4d46ba145b190a67, 0x2fb876099ff41796, 0xc3d07b95527dfea9, 0x3670aa4b9983d42b, 0x6e7628e2f0dfc34a, 0x3eac148ec31121bc, 0x2cade842125ead0b, 0xad1867c9d49e1d6b, 0x73ed9ddff4598d3c, 0x33a34c07cc4064bf, 0x74f2a1bbf0215c11, 0xc8c44bcb870088d1, 0x506faa3e93bb928e, 0x153c57b3e64dca2e, 0x2a9136b6a5338930, 0xa03bf8b5bfd5ca2e, 0x279a0451a0ff97f7, 0x86093059ab595c93, 0x49da43f2bd3edd85, 0x73a5aff6488e7ca9}
,{
0x4c8dd3977194ce69, 0x49edfeff6e32b3d, 0x21b93ed03ea54acd, 0x21188a97197cb726, 0x1cdf73183832c826, 0x334b6755cb76b83e, 0x2377fab63227ed95, 0x37175e5c030de4ea, 0xa84fc7122acba514, 0x3df48f1d75f074b4, 0xa28c8e45eba668ae, 0x3f72707645a1867e, 0x6dccd2f7269d4a4e, 0x9c9e2c63b86c0f4d, 0x50ec8d609601ade5, 0x1512a6c53209bed1, 0x3e394cb92463b8fd, 0x918d2b926cc1907, 0x7c6a9cfc641d67f2, 0x1f2bf8a50c9dd0a3, 0x8d7e3a676eaad866, 0x26583c1bbee5c435, 0x2999d5c8e902f9af, 0x745750534cf8701f, 0xe262771b6c41c745, 0x2ff17fff6648c1c8, 0x90bb3aa4f921468a, 0x5015fddd74518cc7, 0xfdabec3630c9885a, 0xc042426757ab00cd, 0x493197e2d6b3383e, 0xd3b4ca1d478f796, 0xf907bbed256c9e42, 0xa3bed40ea9f25433, 0x57fe705e0b3c35be, 0x9fbffc62416b0961, 0xa63aea267baa6a89, 0x87e82ca17adcc2cf, 0xadc66d5fb02e7f73, 0xa07baee40793ad7f, 0xdf1a60a77c4de3fe, 0x9c6b9805e74d85a3, 0x38dd565348a924a9, 0xf5480b2060a074aa, 0x9ecc9b8fe47e7848, 0xb4e81fb55b35785b, 0x67db454cee5f8644, 0x780b4706c0fd1426, 0x54d05b2862f0acb7, 0x81dd4925231b801a, 0x1138fd5cb3961ab8, 0xa72a98ccf7f0f522, 0x3a4ea7fee739d25d, 0xef42523f8949ac75, 0xc73d7b57c476738, 0xd1e400d03c52b85c, 0x6a3326539541e15d, 0x1f4384ed689516bd, 0xf13b0c9ff062e22e, 0x3b126d061fe75209, 0x148bc5d58ed34f84, 0xffaa0aadfec8c382, 0x1923cd41493432ed, 0x64d626fe835aa1f4}
,{
0x3db1eab0a54ff617, 0x3068f940a8ae8a55, 0x2865f32f5be002a3, 0x6fa90d7344d82be6, 0x11badcea2581daed, 0xd9d4c15e51bb47ce, 0x9a201ad5a309d292, 0x1f584a69c346d447, 0x4cafbb0d0f8cf0be, 0x29021881c1565dd0, 0x14a46daa69920b80, 0x3066a6e4b021c504, 0x500efd286d00a2bb, 0xcae6c0cfc44609, 0x924d7133d87c3a6d, 0xf0578d2309f2deb1, 0x5614051ad70b3f60, 0x45fe51b56cb2babe, 0xa359dfdd49ee2e85, 0xe3617a73796c59f0, 0x1b4829311eca9687, 0xf536ad76016039c6, 0xa551609b6352ec56, 0x6a07d193ab237d12, 0x5edb161169c05ebe, 0x58d78c9fd866e5d4, 0xe471c261989c2a43, 0xe11e4ea21fe989fc, 0x24314f9b45f35db, 0x7db5586f152589fa, 0x43a230679aa54a3a, 0x1eb30d9e8dc8484d, 0x98a5934c4cc2d0ce, 0x4ce229e7a4cd436c, 0x1f70843dd2b504a3, 0x1660e47bcb974fbf, 0xe88386d377daeb7, 0xb9264d0eb3061583, 0x78960ab1ed1eb899, 0xe78a21bfa13af6cc, 0x7a4886ed508c633a, 0x469ce7334c3d6c03, 0xba8300851504955d, 0x7ffb30b4f67a0993, 0x1ddd160a9a67cc10, 0xfafbc948778c4f9f, 0x6a743befe33b1108, 0xf574742bba7dacfa, 0xba5da07a4d3745df, 0xcd01074bb245d0ba, 0xa38e67f82248e081, 0x309d78cabde39215, 0xadd3b5875822344c, 0xc18a5dc0971cec70, 0x7c83e694a989e4b0, 0xabdf4ff0197eddc7, 0x5bd3097ea6c1714f, 0x7cd29e38daf0efd6, 0xdb407ea4317140f2, 0xd0a6d4231bade8a8, 0x495a526c9cb2f7c8, 0xf6590bed29361d7f, 0x431d6aafe2ea8ecd, 0x958f281416553b28}
,{
0x9c7b6e2a19495d50, 0x45efa1658a019940, 0xa21f4d8343dee2e6, 0x37f4912dc77c69f3, 0xd84bfeb975616f52, 0x987168341bb1de35, 0xd9b9418ec2d09730, 0xb36e9eb77682b695, 0xd59df9f24d50075b, 0x72a13bbc8a374305, 0x9799e81d83a50e67, 0x31b2be837ffe185e, 0x70fff980c8f86aaf, 0x10afa00d4e01d064, 0x4b004d5aef8064be, 0x155ed032aa321189, 0xfe86083133f9491e, 0x78ea65c9dc88edd7, 0x7b17862729ca41d5, 0x2a2bd8047c2b7499, 0x6bda198c18032f16, 0xd01244215473a8bc, 0xfadc3cbb6c503fee, 0x1d07a237b178fe05, 0xc11408c5ab497efa, 0xe5d961da4f67998a, 0x5f07f1212c859c62, 0x9698b7fb4486e616, 0x58daebf88510c5b3, 0xd80a5669e36e2b56, 0x9286c2740be8a54f, 0x7aa736f129964f0d, 0xa04cb8b42a677e85, 0xc2d63c1cbe004998, 0x6df325fa35809894, 0x26f0cc3bf45bb38a, 0x172f26d8e4f09a25, 0xccb7c50e2b1382d, 0x2e9aa1062a846800, 0xf3e24422c233b929, 0xe8cec5f5dcdd09, 0x44f3cebf3778199e, 0x418a4aade01716c7, 0xf1ba2f41e3010b6f, 0xdec06fd4d6f56900, 0xd19bb4d26898e7a0, 0xbf230a4ef02fa436, 0xc493ee49daa422e6, 0x83e65c289bd40603, 0xd63f8a120e2f61dc, 0xbbb677e43f96b7a8, 0x49d94552b5171c85, 0xdbc476c925327e4b, 0xbf23929a9d5936f2, 0xa9ab3329db0981b9, 0x499de7bb87258553, 0x43172110385b21a5, 0x391120a68effebb9, 0xd1a95692ed7e06bb, 0x8e611d9cd5c8be4c, 0xbc4da4672cbe4403, 0x93be3a27464144ca, 0xcd4f1b0e5182c667, 0x15b873c77e1edc84}
,{
0x6fb2355401a0cb55, 0x455b176ea07cb63d, 0xc8ed319c1fc074d6, 0x8462d7575809298b, 0x4fdafcd5cdf0a097, 0x8dc923e6f8f99db3, 0xe8cb67b1fc9d38b0, 0x10424dd46d612812, 0x62e24511dc35982c, 0x3ff9f9dd0a344a95, 0x36751b6f86ff22a3, 0x3abeaae2bfd740cd, 0xc8e7b650a4a3a56a, 0xcff4a258f6c5189c, 0x328d34a5ea2dd218, 0x7e109483b7687b99, 0xf057414b774ced77, 0x8c05520663ee2093, 0xefabc7a9b4d1fb22, 0xf7f316e7f37b68df, 0x607bc3c848738d55, 0xef930c2e2fc14359, 0x12c1d6f9771ac1b8, 0x943ef8ced8991f39, 0x3ad5648830fba739, 0xd61ec4bf2cec132b, 0xdd8598b349fe9e25, 0x31ecf17954c7f153, 0xdbbb938294fe0b22, 0xcce98a67ae23df21, 0x4f6774d8819d3ebd, 0x4ffd128f4834015, 0x3ac50b7402798ea7, 0x39c2f1bd8b2ae4f6, 0x518e61636cc01ad3, 0xbfd79d3242588d00, 0xe37801ba3a246811, 0x857fca78297e6d0e, 0x9a5c712c237f2ae8, 0xb66bb09d8b1fb0d7, 0x86c36180745f20ab, 0xb09877b8b29ed4b3, 0xecfb1a9d07017a25, 0x85ed225740d45a80, 0xbdbe9f3e54477257, 0x506fb46341fb4aa0, 0x416feb39f45d850d, 0xc351c1f19611ba2a, 0xde3241835ae72462, 0xdbc0bc88b478c1c1, 0x69b8ecf10c9f996c, 0xdb460bf54c2bfcb0, 0xa968ec140c8abb9a, 0xd4b5e4dda39f5e3a, 0xb071524493e6ab94, 0x9607e6c2cde77b3e, 0x15c78a57da2f119e, 0x57277946366260a7, 0xd57dae3c89aad4c9, 0x29a92902d0d84334, 0x4bada614c115a776, 0xe9818b6c9189d3a3, 0x9540cd2cbc1beefa, 0xcd3a3aa817575a48}
,{
0xa61e277e32764765, 0x16d30a6b91beff8c, 0x7ed7b349f8a481b3, 0x575c117f0d1e62f0, 0x7fafab8fb55049fb, 0x80840c8699336886, 0xc5deb60e9b8e7451, 0x4abb7a50d8828500, 0xdc450dbb945d126f, 0x5e3463f3daecdabf, 0xe43d9570a0c0b975, 0x4af2af925a4cbe90, 0x480e31e7ca23162d, 0x1a41a7b3adae48f3, 0x787c29e4b7a3e7bb, 0x2c54aea636350121, 0x8c4febba3d2703ac, 0x7176a97ad0830130, 0xd51eac34fc2473ad, 0x491f4cadddfbbf01, 0x127530363e3c5c83, 0x146101abaae87bdc, 0x1d8a8c2556d27ef3, 0xde95bbe9f9244cec, 0x85e6eea91af916bc, 0x30cfea5df294c8f5, 0x64f2dae86626f0ce, 0xec8302aba34dbbf5, 0x8d2d1de836481968, 0x61faf27840e75f99, 0x4bcc1e64b2e2cbc5, 0xe9a4d4d65b2e8fa6, 0x3ad6badc791a2875, 0x7dd041da81ecd3c7, 0xc04a12495192749f, 0xc77d50ef9c2e7463, 0x6eaaa5608d3793bd, 0x1deb3794870cb664, 0xee632bf3f32f5e91, 0x480e71bffc9e5a15, 0xf7a0586d7433bd60, 0x8ab2bb3067512382, 0xa3dd662386e1d0ba, 0xffc15854a941705, 0xdf1f6a972c7ccb55, 0x871fca0b7752c2de, 0x44e2c9806ee438cd, 0x17960bb301667706, 0x8e0973da3690183c, 0xc3ac79bf02c1bea8, 0xa59e16eeea833f0d, 0xe54baa4291c05ad8, 0x340945b86c4b3777, 0x26e97899d18f7488, 0xa55e88b577410f7f, 0xb4055b9596634bca, 0xf1ac52a5fc5d207a, 0xe23d4527e2b864de, 0xffc5af710b160c5c, 0xdfe745e5af7c27a0, 0x1dc291c63bd9d560, 0xd14ae5cce6b0244a, 0xc07a7fdaa9d5ddc7, 0xcbfc7d465e9dc8b5}
,{
0x56877bf89fe998c0, 0xda7fb96ca368d36e, 0xcde6b65c77ac12be, 0xab4821359d5c4e61, 0x47122f36d19f86bf, 0xbf4a2a238bfe60f0, 0x877ae5754ac4c753, 0x3fc1f6bcc886699e, 0x418e153fede6a566, 0xd2787b10b60099c3, 0xabfbb3f01e08841c, 0x59b6602299ff51e7, 0xf6bb7a9623974037, 0x133f1a2d365d4ba8, 0x14551fe727034ae8, 0x1a13232a4b38c662, 0x7b37d2cd01a722fc, 0x4366cf374fe87df0, 0x67780b9930e184b5, 0x22187ee65a4cbf3f, 0x730e7725277c87e0, 0x772410aa3d888485, 0xe3be490efe9744e0, 0xd2f3c2184bb9b360, 0xd410dc7869604cc1, 0xe514b7c68200f12a, 0x2576990561d959c, 0x87c30322a2b8163c, 0x67774d78db0e43c6, 0xb4e69cac47582700, 0x913cc5e8b6d941a6, 0xc7e0a89aa38f1fe, 0xab49863d1268b32d, 0xa3265383b7e69449, 0xe82e447c5d17cf38, 0x6c49f103087cc1f3, 0x9c0f1e1c544b5868, 0xd8b5f3b5dafcd671, 0x1536f22d54b4203c, 0x99b2a219f74f7123, 0x57477cdcbd7a0269, 0xbc6aa2361f2f4c4b, 0x65395310bcd9ebc3, 0x9b4ec45c1ae1213e, 0x2f2b282f19b6973a, 0x4d5427da9c405a9a, 0x8503dff2fee404b6, 0xaf48d81e11409dbb, 0xb033e67d7aef98d2, 0x8c2aecc5f6aa66d1, 0x8f6db5ad5a62a1cc, 0x70524dcab4759f3d, 0x70ed640602947d24, 0x826601deeec8cc1d, 0x54faeb4bc8381abc, 0x9e7e24c3e93a4337, 0xbc8d574a6f05ed7a, 0xcd7af15b85a24ea0, 0x2189340056e14eb5, 0x868353d4f454dbd0, 0x10d431ca5f29c101, 0x7cc2bf584153d100, 0x3c8421e881d1280c, 0x9e6ec3230937c0a}

};
constexpr U64 SideKey = 0x8fb61a135e1bc6a6;
constexpr U64 CastleKeys[16] = {
0x78891381fb1a5022, 0xa8222ea3a5f77135, 0x728f580a9f63641f, 0xdd4a7c1b68ff3221, 0xc374cd80fb6930, 0x1c285eb5d560eef6, 0xe43dc5a8ae0cb22c, 0xba856cee00f2c050, 0xb9ee304375106de2, 0x70b744beabf873d8, 0x66722023a98e7b12, 0xe5378eabfb1f66c8, 0xa69473d5f4714afb, 0x5a0197d676be0de, 0x493406a03b12af9b, 0xedd6f1d4a0eda2a7
};

//// hash


#include <atomic>

typedef struct S_HASHENTRY {
	U64 lastWrite = 0ULL;
	U64 posKey = 0ULL;
	short depth = 0;
	BoardValue score = 0;
	U64 move = 0ULL;
	short flags = 0;
	std::atomic_flag lock;
} S_HASHENTRY;

typedef struct S_HASHTABLE {
	S_HASHENTRY* entrys = nullptr;
	std::atomic<U64> numEntries = 0;
	std::atomic<U64> newWrite = 0;
	std::atomic<U64> overWrite = 0;
	std::atomic<U64> hit = 0;
	std::atomic<U64> cut = 0;
} S_HASHTABLE;

typedef struct {

	std::atomic<U64> starttime;
	std::atomic<U64> stoptime;
	std::atomic<int> depth;
	std::atomic<bool> timeset;
	std::atomic<int> movestogo;

	std::atomic<U64> nodes;

	std::atomic<int> SubSearchNum;

	std::atomic<bool> useBook;
	std::atomic<bool> quit;
	std::atomic<bool> stopped;

	std::atomic<int> fh;
	std::atomic<int> fhf;
	std::atomic<int> nullCut;

} S_SEARCHINFO;

static S_HASHTABLE* _pHashTable = nullptr;
static S_SEARCHINFO* _pSearchInfo = nullptr;
static S_MemoryFrame _MemoryFrame;

typedef enum HashFlags {
	HFNONE, HFALPHA, HFBETA, HFEXACT
} HashFlags;

inline S_HASHENTRY* GET_NEW_HASHENTRY(const U64 posKey) {
	U64 id = posKey % _pHashTable->numEntries;
	U64 timeExpire = GetMilliTime() - 30000;

	while (id < _pHashTable->numEntries &&
		_pHashTable->entrys[id].posKey != 0ULL &&
		(_pHashTable->entrys[id].posKey != posKey || _pHashTable->entrys[id].lastWrite < timeExpire)
		)
		id++;

	return &(_pHashTable->entrys[id]);
}

inline S_HASHENTRY* GET_HASHENTRY(const U64 posKey) {
	U64 id = posKey % _pHashTable->numEntries;

	while (id < _pHashTable->numEntries &&
		_pHashTable->entrys[id].posKey != 0ULL &&
		_pHashTable->entrys[id].posKey != posKey
		)
		id++;

	return &(_pHashTable->entrys[id]);
}

inline void ProbeHashEntry(const U64 PosKey, short* depth, BoardValue* score) {
	S_HASHENTRY* hashEntry = GET_HASHENTRY(PosKey);

	while (hashEntry->lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin

	if (hashEntry->posKey == PosKey) {
		*depth = hashEntry->depth;
		*score = hashEntry->score;
	}
	else {
		*depth = 0;
	}
	hashEntry->lock.clear(std::memory_order_release);
}

inline bool ProbeHashEntry(const U64 PosKey, const short Ply, U64* move, BoardValue* score, const BoardValue alpha, const BoardValue beta, const short depth) {
	S_HASHENTRY* hashEntry = GET_HASHENTRY(PosKey);

	while (hashEntry->lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin

	if (hashEntry->posKey == PosKey) {
		*move = hashEntry->move;
		if (hashEntry->depth >= depth) {
			_pHashTable->hit++;

			*score = hashEntry->score;
			if (*score > MAX_MATE) *score -= Ply;
			else if (*score < MIN_MATE) *score += Ply;

			short flags = hashEntry->flags;
			hashEntry->lock.clear(std::memory_order_release);
			switch (flags) {
			case HFALPHA:
				if (*score <= alpha) {
					*score = alpha;
					return true;
				}
				break;
			case HFBETA:
				if (*score >= beta) {
					*score = beta;
					return true;
				}
				break;
			case HFEXACT:
				return true;
				break;
			default: ASSERT(false); return false; break;
			}
		}
	}

	hashEntry->lock.clear(std::memory_order_release);
	return false;
}

inline void StoreHashEntry(const U64 PosKey, const short Ply, const U64 move, const BoardValue score, const short flags, const short depth) {
	S_HASHENTRY* hashEntry = GET_NEW_HASHENTRY(PosKey);

	while (hashEntry->lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin

	hashEntry->lastWrite = GetMilliTime();

	if (hashEntry->posKey == 0ULL)// new write
		_pHashTable->newWrite++;
	else if (hashEntry->posKey == PosKey) {// overwrite but same position
		if (hashEntry->depth > depth)
			return;// abort if stored depth is greater then new one
		// new depth is bigger or same
	}
	else// overwrite old entry
		_pHashTable->overWrite++;	

	if (score > MAX_MATE) hashEntry->score = score + Ply;
	else if (score < MIN_MATE) hashEntry->score = score - Ply;
	else hashEntry->score = score;

	hashEntry->move = move;
	hashEntry->posKey = PosKey;
	hashEntry->flags = flags;
	hashEntry->depth = depth;
	hashEntry->lock.clear(std::memory_order_release);
}

inline U64 ProbePvMove(const U64 posKey) {
	S_HASHENTRY* hashEntry = GET_HASHENTRY(posKey);

	while (hashEntry->lock.test_and_set(std::memory_order_acquire)) // acquire lock
		; // spin

	if (hashEntry->posKey == posKey) {
		hashEntry->lock.clear(std::memory_order_release);
		return hashEntry->move;
	}
	hashEntry->lock.clear(std::memory_order_release);
	return NOMOVE;
}

inline void ClearHashTable() {
	for (U64 i = 0ULL; i < _pHashTable->numEntries; i++) {
		S_HASHENTRY* tableEntry = &_pHashTable->entrys[i];
		tableEntry->posKey = tableEntry->move = 0ULL;
		tableEntry->depth = tableEntry->score = tableEntry->flags = 0;

		tableEntry->lock.clear();
	}
	_pHashTable->newWrite = _pHashTable->overWrite = _pHashTable->hit = _pHashTable->cut = 0;
}

inline void InitSearchinfo() {
	_pSearchInfo = new S_SEARCHINFO();
}

inline void destroyHashTable() {
	if (_pHashTable != nullptr) {
		if (_pHashTable->entrys != nullptr) {
			_ReleaseMemoryFrame(&_MemoryFrame);
		}
		delete _pHashTable;
		delete _pSearchInfo;
		_pHashTable = nullptr;
		InitSearchinfo();
	}
}

inline void InitHashTable(const U64 HashSize) {
	destroyHashTable();
	_pHashTable = new S_HASHTABLE();

	_pHashTable->numEntries = HashSize / sizeof(S_HASHENTRY);
	_pHashTable->numEntries -= 2;

	_MemoryFrame = _GetMemoryFrame(HashHeap);
	_pHashTable->entrys = (S_HASHENTRY*)_AllocFrameMemory<HashHeap>(((int)_pHashTable->numEntries) * sizeof(S_HASHENTRY));
	if (_pHashTable->entrys == nullptr) {
		print_console("Hash Allocation Failed, trying %dBytes...\n", HashSize / 2);
		InitHashTable(HashSize / 2);
	}
	else {
		ClearHashTable();
		print_console("HashTable init complete with %d entries\n", (int)_pHashTable->numEntries);
	}
}
