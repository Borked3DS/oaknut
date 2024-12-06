// SPDX-FileCopyrightText: Copyright (c) 2023 merryhime <https://mary.rs>
// SPDX-License-Identifier: MIT

void AUTDA(XReg xd, XRegSp xn)
{
    emitOp<"110110101100000100Z110nnnnnddddd", "Z", "d", "n">(0, xd, xn);
}
void AUTDB(XReg xd, XRegSp xn)
{
    emitOp<"110110101100000100Z111nnnnnddddd", "Z", "d", "n">(0, xd, xn);
}
void AUTDZA(XReg xd)
{
    emitOp<"110110101100000100Z11011111ddddd", "Z", "d">(1, xd);
}
void AUTDZB(XReg xd)
{
    emitOp<"110110101100000100Z11111111ddddd", "Z", "d">(1, xd);
}
void AUTIA(XReg xd, XRegSp xn)
{
    emitOp<"110110101100000100Z100nnnnnddddd", "Z", "d", "n">(0, xd, xn);
}
void AUTIB(XReg xd, XRegSp xn)
{
    emitOp<"110110101100000100Z101nnnnnddddd", "Z", "d", "n">(0, xd, xn);
}
void AUTIZA(XReg xd)
{
    emitOp<"110110101100000100Z10011111ddddd", "Z", "d">(1, xd);
}
void AUTIZB(XReg xd)
{
    emitOp<"110110101100000100Z10111111ddddd", "Z", "d">(1, xd);
}
void BLRAA(XReg xn, XRegSp xm)
{
    emitOp<"1101011Z0011111100001Mnnnnnmmmmm", "Z", "M", "n", "m">(1, 0, xn, xm);
}
void BLRAAZ(XReg xn)
{
    emitOp<"1101011Z0011111100001Mnnnnn11111", "Z", "M", "n">(0, 0, xn);
}
void BLRAB(XReg xn, XRegSp xm)
{
    emitOp<"1101011Z0011111100001Mnnnnnmmmmm", "Z", "M", "n", "m">(1, 1, xn, xm);
}
void BLRABZ(XReg xn)
{
    emitOp<"1101011Z0011111100001Mnnnnn11111", "Z", "M", "n">(0, 1, xn);
}
void BRAA(XReg xn, XRegSp xm)
{
    emitOp<"1101011Z0001111100001Mnnnnnmmmmm", "Z", "M", "n", "m">(1, 0, xn, xm);
}
void BRAAZ(XReg xn)
{
    emitOp<"1101011Z0001111100001Mnnnnn11111", "Z", "M", "n">(0, 0, xn);
}
void BRAB(XReg xn, XRegSp xm)
{
    emitOp<"1101011Z0001111100001Mnnnnnmmmmm", "Z", "M", "n", "m">(1, 1, xn, xm);
}
void BRABZ(XReg xn)
{
    emitOp<"1101011Z0001111100001Mnnnnn11111", "Z", "M", "n">(0, 1, xn);
}
void ERETAA()
{
    emitOp<"110101101001111100001M1111111111", "M">(0);
}
void ERETAB()
{
    emitOp<"110101101001111100001M1111111111", "M">(1);
}
void LDAPR(WReg wt, XRegSp xn)
{
    emitOp<"1011100010111111110000nnnnnttttt", "t", "n">(wt, xn);
}
void LDAPR(XReg xt, XRegSp xn)
{
    emitOp<"1111100010111111110000nnnnnttttt", "t", "n">(xt, xn);
}
void LDAPRB(WReg wt, XRegSp xn)
{
    emitOp<"0011100010111111110000nnnnnttttt", "t", "n">(wt, xn);
}
void LDAPRH(WReg wt, XRegSp xn)
{
    emitOp<"0111100010111111110000nnnnnttttt", "t", "n">(wt, xn);
}
void LDRAA(XReg xt, XRegSp xn, SOffset<13, 3> simm = 0)
{
    emitOp<"11111000Mi1iiiiiiiiiW1nnnnnttttt", "M", "W", "t", "n", "i">(0, 0, xt, xn, simm);
}
void LDRAB(XReg xt, XRegSp xn, SOffset<13, 3> simm = 0)
{
    emitOp<"11111000Mi1iiiiiiiiiW1nnnnnttttt", "M", "W", "t", "n", "i">(1, 0, xt, xn, simm);
}
void LDRAA(XReg xt, XRegSp xn, PreIndexed, SOffset<13, 3> simm)
{
    emitOp<"11111000Mi1iiiiiiiiiW1nnnnnttttt", "M", "W", "t", "n", "i">(0, 1, xt, xn, simm);
}
void LDRAB(XReg xt, XRegSp xn, PreIndexed, SOffset<13, 3> simm)
{
    emitOp<"11111000Mi1iiiiiiiiiW1nnnnnttttt", "M", "W", "t", "n", "i">(1, 1, xt, xn, simm);
}
void PACDA(XReg xd, XRegSp xn)
{
    emitOp<"110110101100000100Z010nnnnnddddd", "Z", "d", "n">(0, xd, xn);
}
void PACDB(XReg xd, XRegSp xn)
{
    emitOp<"110110101100000100Z011nnnnnddddd", "Z", "d", "n">(0, xd, xn);
}
void PACDZA(XReg xd)
{
    emitOp<"110110101100000100Z01011111ddddd", "Z", "d">(1, xd);
}
void PACDZB(XReg xd)
{
    emitOp<"110110101100000100Z01111111ddddd", "Z", "d">(1, xd);
}
void PACGA(XReg xd, XReg xn, XRegSp xm)
{
    emitOp<"10011010110mmmmm001100nnnnnddddd", "d", "n", "m">(xd, xn, xm);
}
void PACIA(XReg xd, XRegSp xn)
{
    emitOp<"110110101100000100Z000nnnnnddddd", "Z", "d", "n">(0, xd, xn);
}
void PACIB(XReg xd, XRegSp xn)
{
    emitOp<"110110101100000100Z001nnnnnddddd", "Z", "d", "n">(0, xd, xn);
}
void PACIZA(XReg xd)
{
    emitOp<"110110101100000100Z00011111ddddd", "Z", "d">(1, xd);
}
void PACIZB(XReg xd)
{
    emitOp<"110110101100000100Z00111111ddddd", "Z", "d">(1, xd);
}
void RETAA()
{
    emitOp<"110101100101111100001M1111111111", "M">(0);
}
void RETAB()
{
    emitOp<"110101100101111100001M1111111111", "M">(1);
}
void XPACD(XReg xd)
{
    emitOp<"110110101100000101000D11111nnnnn", "D", "n">(1, xd);
}
void XPACI(XReg xd)
{
    emitOp<"110110101100000101000D11111nnnnn", "D", "n">(0, xd);
}