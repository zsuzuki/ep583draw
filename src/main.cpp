//
// copyright: Suzuki Yoshinori(wave.suzuki.z@gmail.com)
// since: 2023/02/22
//
#include <iostream>
#include <nlohmann/json.hpp>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <sys/time.h>
#include <vector>
extern "C"
{
#include <DEV_Config.h>
#include <EPD_5in83b_V2.h>
#include <GUI_Paint.h>
}
#include "font.h"

namespace
{

// 文字列描画
void
DrawString(uint16_t x, uint16_t y, uint16_t col, const char* str, const Katakori::Font* font)
{
  if (str == nullptr || font == nullptr || font->GetFontSize() > 64)
    return;

  font->GetString(str,
                  [&](const Katakori::FontBitmap& bitmap)
                  {
                    if (bitmap.data == nullptr)
                    {
                      x += bitmap.pitch + 1;
                      return;
                    }
                    int width   = bitmap.width;
                    int height  = bitmap.height;
                    int dx      = x + bitmap.offsetX;
                    int dy      = y + bitmap.offsetY;
                    int xoffset = 0;

                    auto* fontdata = bitmap.data;

                    for (int ly = 0; ly < height; ly++)
                    {
                      uint64_t line = bitmap.GetLine(fontdata);
                      uint64_t mask = bitmap.GetMask();
                      for (int lx = 0; lx < width; lx++)
                      {
                        if (line & mask)
                        {
                          Paint_SetPixel(dx + lx, dy + ly, col);
                          if (lx == width - 1)
                            xoffset = 1;
                        }
                        line <<= 1UL;
                      }
                    }
                    x += bitmap.pitch + xoffset;
                  });
}

} // namespace

//
//
//
int
main(int argc, char** argv)
{
  set_sys_clock_khz(133000, true);
  if (DEV_Module_Init() != 0)
  {
    return -1;
  }
  setup_default_uart();

  EPD_5IN83B_V2_Init();
  EPD_5IN83B_V2_Clear();
  DEV_Delay_ms(500);

  std::vector<UBYTE> blImage, ryImage;
  UWORD              imgSize =
      ((EPD_5IN83B_V2_WIDTH % 8 == 0) ? (EPD_5IN83B_V2_WIDTH / 8) : (EPD_5IN83B_V2_WIDTH / 8 + 1)) * EPD_5IN83B_V2_HEIGHT;
  blImage.resize(imgSize);
  ryImage.resize(imgSize);
  Paint_NewImage(blImage.data(), EPD_5IN83B_V2_WIDTH, EPD_5IN83B_V2_HEIGHT, 0, WHITE);
  Paint_NewImage(ryImage.data(), EPD_5IN83B_V2_WIDTH, EPD_5IN83B_V2_HEIGHT, 0, WHITE);

  Katakori::Font myrc, ibmm, kpop, logo, pop;
  if (!myrc.AssignMappedMemory((const void*)(0x10000000 + 0x100000)))
    printf("font: [Myrica] load failed\n");
  if (!ibmm.AssignMappedMemory((const void*)(0x10000000 + 0x130000)))
    printf("font: [IBMPlex-Medium] load failed\n");
  if (!kpop.AssignMappedMemory((const void*)(0x10000000 + 0x160000)))
    printf("font: [DFKakuPop] load failed\n");
  if (!logo.AssignMappedMemory((const void*)(0x10000000 + 0x190000)))
    printf("font: [07LogoType] load failed\n");
  if (!pop.AssignMappedMemory((const void*)(0x10000000 + 0x1C0000)))
    printf("font: [DFPopMix] load failed\n");
  {
    Paint_SelectImage(blImage.data());
    Paint_Clear(WHITE);
    Paint_SelectImage(ryImage.data());
    Paint_Clear(WHITE);
    DrawString(80, 50, BLACK, "コマンドを転送してください(Myrica)", &myrc);
    DrawString(120, 75, BLACK, " # req_epd <list.toml>", &myrc);
    DrawString(80, 100, BLACK, "コマンドを転送してください(IBMPlexJP)", &ibmm);
    DrawString(120, 125, BLACK, " # req_epd <list.toml>", &ibmm);
    DrawString(80, 150, BLACK, "コマンドを転送してください(KakuPop)", &kpop);
    DrawString(120, 175, BLACK, " # req_epd <list.toml>", &kpop);
    DrawString(80, 200, BLACK, "コマンドを転送してください(LogoType)", &logo);
    DrawString(120, 225, BLACK, " # req_epd <list.toml>", &logo);
    DrawString(80, 250, BLACK, "コマンドを転送してください(Pop Gothic)", &pop);
    DrawString(120, 275, BLACK, " # req_epd <list.toml>", &pop);
    EPD_5IN83B_V2_Display(blImage.data(), ryImage.data());
  }

  while (true)
  {
    Paint_SelectImage(blImage.data());
    Paint_Clear(WHITE);
    Paint_SelectImage(ryImage.data());
    Paint_Clear(WHITE);

    // EPD_5IN83B_V2_Display(blImage.data(), ryImage.data());
    Paint_SelectImage(blImage.data());

    bool wait = true;
    int  x = 0, y = 0, r = 100;
    int  w = EPD_5IN83B_V2_WIDTH, h = EPD_5IN83B_V2_HEIGHT;
    do
    {
      char   buff[512]{};
      size_t readSize = 0;

      printf("[standby]\n");
      bool failed = true;
      do
      {
        char* ptr = buff + readSize;
        std::cin.getline(ptr, sizeof(buff) - readSize);
        int len = strlen(ptr);
        if (len == 0)
        {
          // no read
          printf("[empty]\n");
          break;
        }
        ptr[len] = '\n';
        readSize += len + 1;
        nlohmann::json json = nlohmann::json::parse(buff, nullptr, false);
        std::string    d    = json.dump(2);
        if (d == "<discarded>")
        {
          printf("[continue]\n");
          continue;
        }
        int ID = 0;
        if (json["ID"].is_null() == false)
        {
          ID = json["ID"].get<int>();
        }
        if (json["x"].is_null() == false)
        {
          x = json["x"].get<int>();
        }
        if (json["y"].is_null() == false)
        {
          y = json["y"].get<int>();
        }
        if (json["w"].is_null() == false)
        {
          w = json["w"].get<int>();
        }
        if (json["h"].is_null() == false)
        {
          h = json["h"].get<int>();
        }
        if (json["r"].is_null() == false)
        {
          r = json["r"].get<int>();
        }
        if (json["layer"].is_null() == false)
        {
          std::string layer = json["layer"].get<std::string>();
          if (layer == "BLACK")
            Paint_SelectImage(blImage.data());
          else if (layer == "RED")
            Paint_SelectImage(ryImage.data());
        }
        if (json["clear"].is_null() == false && json["clear"].get<bool>())
        {
          Paint_Clear(WHITE);
        }
        if (json["draw"].is_null() == false)
        {
          std::string draw = json["draw"].get<std::string>();
          if (draw == "fill")
            Paint_DrawRectangle(x, y, x + w - 1, y + h - 1, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          else if (draw == "rect")
            Paint_DrawRectangle(x, y, x + w - 1, y + h - 1, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
          else if (draw == "circle")
            Paint_DrawCircle(x, y, r, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
          else if (draw == "fillcircle")
            Paint_DrawCircle(x, y, r, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
          else if (draw == "line")
          {
            auto ls = LINE_STYLE_SOLID;
            auto wd = DOT_PIXEL_1X1;
            if (json["style"].is_null() == false)
            {
              std::string s = json["style"].get<std::string>();
              if (s == "DOT")
                ls = LINE_STYLE_DOTTED;
            }
            if (json["size"].is_null() == false)
            {
              decltype(wd) wl[] = {DOT_PIXEL_1X1, DOT_PIXEL_1X1, DOT_PIXEL_2X2, DOT_PIXEL_3X3, DOT_PIXEL_4X4,
                                   DOT_PIXEL_5X5, DOT_PIXEL_6X6, DOT_PIXEL_7X7, DOT_PIXEL_8X8};
              wd                = wl[json["size"].get<int>()];
            }
            Paint_DrawLine(x, y, x + w - 1, y + h - 1, BLACK, wd, ls);
          }
        }
        if (json["ascii"].is_null() == false)
        {
          std::string     msg  = json["ascii"].get<std::string>();
          Katakori::Font* font = &logo;
          DrawString(x, y, BLACK, msg.c_str(), font);
        }
        if (json["message"].is_null() == false)
        {
          Katakori::Font* font = &kpop;
          if (json["font"].is_null() == false)
          {
            int n = json["font"].get<int>();
            if (n == 2)
              font = &myrc;
            else if (n == 3)
              font = &pop;
            else if (n == 4)
              font = &logo;
            else if (n == 5)
              font = &ibmm;
          }
          std::string msg = json["message"].get<std::string>();
          // printEFont(x, y, msg.c_str(), BLACK, WHITE);
          DrawString(x, y, BLACK, msg.c_str(), font);
        }
        if (json["flash"].is_null() == false && json["flash"].get<bool>())
        {
          EPD_5IN83B_V2_Display(blImage.data(), ryImage.data());
        }
        printf("[accept:%d]\n", ID);
        // DEV_Delay_ms(100);
        failed = false;
      } while (failed);
    } while (wait);
  }

  DEV_Module_Exit();

  return 0;
}
