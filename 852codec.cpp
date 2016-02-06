#include "852codec.h"

QIBM852Codec::QIBM852Codec()
{
    IBM850Codec = QTextCodec::codecForName("IBM850");
}

QStringList QIBM852Codec::aliases()
{
    QStringList ret;
    ret << "ibm-852_P100-1995"
        << "ibm-852"
        << "IBM852"
        << "cp852"
        << "852"
        << "csPCp852"
        << "windows-852";
    return ret;
}

QString QIBM852Codec::convertToUnicode(const char *in, int length, ConverterState *state) const
{
    if (IBM850Codec == NULL) {
        return "";
    } else {
        return IBM850Codec->toUnicode(in, length, state);
    }
}

QByteArray QIBM852Codec::convertFromUnicode(const QChar *in, int length, ConverterState *) const
{
    if (IBM850Codec == NULL) {
        return "";
    } else {
        QByteArray ret;
        for (int i = 0; i < length; i++) {
            switch (in[i].unicode()) {
                //Ç
                case 199:
                ret.append(128);
                break;
                //Ü
                case 252:
                ret.append(129);
                break;
                //É
                case 233:
                ret.append(130);
                break;
                //Â
                case 226:
                ret.append(131);
                break;
                //Ä
                case 228:
                ret.append(132);
                break;
                //Ů
                case 367:
                ret.append(133);
                break;
                //Ć
                case 263:
                ret.append(134);
                break;
                //Ç
                case 231:
                ret.append(135);
                break;
                //Ł
                case 322:
                ret.append(136);
                break;
                //Ë
                case 235:
                ret.append(137);
                break;
                //Ő
                case 336:
                ret.append(138);
                break;
                //Ő
                case 337:
                ret.append(139);
                break;
                //Î
                case 238:
                ret.append(140);
                break;
                //Ź
                case 377:
                ret.append(141);
                break;
                //Ä
                case 196:
                ret.append(142);
                break;
                //Ć
                case 262:
                ret.append(143);
                break;
                //É
                case 201:
                ret.append(144);
                break;
                //Ĺ
                case 313:
                ret.append(145);
                break;
                //Ĺ
                case 314:
                ret.append(146);
                break;
                //Ô
                case 244:
                ret.append(147);
                break;
                //Ö
                case 246:
                ret.append(148);
                break;
                //Ľ
                case 317:
                ret.append(149);
                break;
                //Ľ
                case 318:
                ret.append(150);
                break;
                //Ś
                case 346:
                ret.append(151);
                break;
                //Ś
                case 347:
                ret.append(152);
                break;
                //Ö
                case 214:
                ret.append(153);
                break;
                //Ü
                case 220:
                ret.append(154);
                break;
                //Ť
                case 356:
                ret.append(155);
                break;
                //Ť
                case 357:
                ret.append(156);
                break;
                //Ł
                case 321:
                ret.append(157);
                break;
                //×
                case 215:
                ret.append(158);
                break;
                //Č
                case 269:
                ret.append(159);
                break;
                //Á
                case 225:
                ret.append(160);
                break;
                //Í
                case 237:
                ret.append(161);
                break;
                //Ó
                case 243:
                ret.append(162);
                break;
                //Ú
                case 250:
                ret.append(163);
                break;
                //Ą
                case 260:
                ret.append(164);
                break;
                //Ą
                case 261:
                ret.append(165);
                break;
                //Ž
                case 381:
                ret.append(166);
                break;
                //Ž
                case 382:
                ret.append(167);
                break;
                //Ę
                case 280:
                ret.append(168);
                break;
                //Ę
                case 281:
                ret.append(169);
                break;
                //¬
                case 172:
                ret.append(170);
                break;
                //Ź
                case 378:
                ret.append(171);
                break;
                //Č
                case 268:
                ret.append(172);
                break;
                //Ş
                case 351:
                ret.append(173);
                break;
                //«
                case 171:
                ret.append(174);
                break;
                //»
                case 187:
                ret.append(175);
                break;
                //░
                case 9617:
                ret.append(176);
                break;
                //▒
                case 9618:
                ret.append(177);
                break;
                //▓
                case 9619:
                ret.append(178);
                break;
                //│
                case 9474:
                ret.append(179);
                break;
                //┤
                case 9508:
                ret.append(180);
                break;
                //Á
                case 193:
                ret.append(181);
                break;
                //Â
                case 194:
                ret.append(182);
                break;
                //Ě
                case 282:
                ret.append(183);
                break;
                //Ş
                case 350:
                ret.append(184);
                break;
                //╣
                case 9571:
                ret.append(185);
                break;
                //║
                case 9553:
                ret.append(186);
                break;
                //╗
                case 9559:
                ret.append(187);
                break;
                //╝
                case 9565:
                ret.append(188);
                break;
                //Ż
                case 379:
                ret.append(189);
                break;
                //Ż
                case 380:
                ret.append(190);
                break;
                //┐
                case 9488:
                ret.append(191);
                break;
                //└
                case 9492:
                ret.append(192);
                break;
                //┴
                case 9524:
                ret.append(193);
                break;
                //┬
                case 9516:
                ret.append(194);
                break;
                //├
                case 9500:
                ret.append(195);
                break;
                //─
                case 9472:
                ret.append(196);
                break;
                //┼
                case 9532:
                ret.append(197);
                break;
                //Ă
                case 258:
                ret.append(198);
                break;
                //Ă
                case 259:
                ret.append(199);
                break;
                //╚
                case 9562:
                ret.append(200);
                break;
                //╔
                case 9556:
                ret.append(201);
                break;
                //╩
                case 9577:
                ret.append(202);
                break;
                //╦
                case 9574:
                ret.append(203);
                break;
                //╠
                case 9568:
                ret.append(204);
                break;
                //═
                case 9552:
                ret.append(205);
                break;
                //╬
                case 9580:
                ret.append(206);
                break;
                //¤
                case 164:
                ret.append(207);
                break;
                //Đ
                case 273:
                ret.append(208);
                break;
                //Đ
                case 272:
                ret.append(209);
                break;
                //Ď
                case 270:
                ret.append(210);
                break;
                //Ë
                case 203:
                ret.append(211);
                break;
                //Ď
                case 271:
                ret.append(212);
                break;
                //Ň
                case 327:
                ret.append(213);
                break;
                //Í
                case 205:
                ret.append(214);
                break;
                //Î
                case 206:
                ret.append(215);
                break;
                //Ě
                case 283:
                ret.append(216);
                break;
                //┘
                case 9496:
                ret.append(217);
                break;
                //┌
                case 9484:
                ret.append(218);
                break;
                //█
                case 9608:
                ret.append(219);
                break;
                //▄
                case 9604:
                ret.append(220);
                break;
                //Ţ
                case 354:
                ret.append(221);
                break;
                //Ů
                case 366:
                ret.append(222);
                break;
                //▀
                case 9600:
                ret.append(223);
                break;
                //Ó
                case 211:
                ret.append(224);
                break;
                //ß
                case 223:
                ret.append(225);
                break;
                //Ô
                case 212:
                ret.append(226);
                break;
                //Ń
                case 323:
                ret.append(227);
                break;
                //Ń
                case 324:
                ret.append(228);
                break;
                //Ň
                case 328:
                ret.append(229);
                break;
                //Š
                case 352:
                ret.append(230);
                break;
                //Š
                case 353:
                ret.append(231);
                break;
                //Ŕ
                case 340:
                ret.append(232);
                break;
                //Ú
                case 218:
                ret.append(233);
                break;
                //Ŕ
                case 341:
                ret.append(234);
                break;
                //Ű
                case 368:
                ret.append(235);
                break;
                //Ý
                case 253:
                ret.append(236);
                break;
                //Ý
                case 221:
                ret.append(237);
                break;
                //Ţ
                case 355:
                ret.append(238);
                break;
                //´
                case 180:
                ret.append(239);
                break;
                //Soft hyphen
                case 173:
                ret.append(240);
                break;
                //˝
                case 733:
                ret.append(241);
                break;
                //˛
                case 731:
                ret.append(242);
                break;
                //ˇ
                case 711:
                ret.append(243);
                break;
                //˘
                case 728:
                ret.append(244);
                break;
                //§
                case 167:
                ret.append(245);
                break;
                //÷
                case 247:
                ret.append(246);
                break;
                //¸
                case 184:
                ret.append(247);
                break;
                //°
                case 176:
                ret.append(248);
                break;
                //¨
                case 168:
                ret.append(249);
                break;
                //˙
                case 729:
                ret.append(250);
                break;
                //Ű
                case 369:
                ret.append(251);
                break;
                //Ř
                case 344:
                ret.append(252);
                break;
                //Ř
                case 345:
                ret.append(253);
                break;
                //■
                case 9632:
                ret.append(254);
                break;
                //Non-breaking space
                case 160:
                ret.append(255);
                break;
                default:
                    ret.append(IBM850Codec->fromUnicode(&in[i], 1));
                    break;
            }
        }
    return ret;
    }
}

QIBM852Codec::~QIBM852Codec()
{

}
