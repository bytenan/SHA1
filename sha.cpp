#include <iostream>
#include <string>
#include <cstring>

const unsigned int h[] = { 0x67452301,0xefcdab89,0x98badcfe,0x10325476,0xc3d2e1f0 };

void CharToUlong (const unsigned char *char_data, unsigned int *ulong_data) 
{
    for (int i = 0; i < 16; i++)
    {
        int j = 4 * i;
        ulong_data[i] = ((unsigned int)char_data[j]) << 24 | 
                ((unsigned int)char_data[1 + j]) << 16 | 
                ((unsigned int)char_data[2 + j]) << 8 |
                ((unsigned int)char_data[3 + j]) << 0;
    }
    for (int i = 16; i < 80; i++)
    {
        ulong_data[i] = ulong_data[i - 16] ^ ulong_data[i - 14] ^ 
                ulong_data[i - 8] ^ ulong_data[i - 3];
        ulong_data[i] = (ulong_data[i] << 1) | (ulong_data[i] >> 31);
    }
}

int main() {


    std::string input;
    std::cin >> input;
  
    size_t message_length = ((input.size() + 8) / 64) * 64 + 56 + 8;// 求消息的总长度
    unsigned char *message = (unsigned char *)malloc(message_length);
    memset(message, 0x00, message_length);// 将消息初始化为0
    memcpy(message, input.c_str(), input.size());// 将输入填充进消息
    message[message_length] = 0x80;// 将1填充进消息

    unsigned char input_length[8];// 长度的空间为8个字节
    memset(input_length, 0x00, 8);// 初始化为0
    unsigned int temp_length = (unsigned int)(input.size() * 8);// 将长度转化为bit
    memcpy(input_length, &temp_length, sizeof(unsigned long));// 填入长度

    /*
    *颠倒数据长度存储的端序
    */
    for (int i = 7; i >= 4; i--)
    {
      int y = input_length[i];
      input_length[i] = input_length[7-i];
      input_length[7-i] = y;
    }
    memcpy(message + (message_length - 8), input_length, 8);// 将长度填充进消息


    // 每次循环都是以一个分组为单位
    for (int i = 0; i < message_length; i += 64)
    {
        //A=H[0],B=H[1],C=H[2],D=H[3],E=H[4]
        unsigned int H[5] = { 0 };// 五个缓冲区
        unsigned char group[64] = { 0 };// 以分组为单位，每个分组512bit，即64byte（16个字）
        unsigned int W[80] = { 0 };// 将每个分组的16个字拓展为80个字

        memset(group, 0, 64);
        memset(W, 0, 80 * sizeof(unsigned int));
        memcpy(H, h, 5 * sizeof(unsigned int));
        memcpy(group, (message + i), 64);// 从消息中取出分组

        CharToUlong(group, W);// 拓展字函数

        int k = 0;
        int f = 0;
        for (int j = 0; j < 80; j++)
        {
            switch ((int)j / 20)
            {
            case 0:
                k = 0x5a827999;
                f = (H[1] & H[2]) | ((~H[1]) & H[3]);
                break;
            case 1:
                k = 0x6ed9eba1;
                f = H[1] ^ H[2] ^ H[3];
                break;
            case 2:
                k = 0x8f1bbcdc;
                f = (H[1] & H[2]) | (H[1] & H[3]) | (H[2] & H[3]);
                break;
            case 3:
                k = 0xca62c1d6;
                f = H[1] ^ H[2] ^ H[3];
                break;
            default:
                break;
            }
            //ABCDE位置移动
            int temp_A = ((H[0] << 5) | (H[0] >> 27)) + f + H[4] + W[j] + k;
            H[4] = H[3];
            H[3] = H[2];
            H[2] = (H[1] << 30) | (H[1] >> 2);
            H[1] = H[0];
            H[0] = temp_A;
        }
        //ABCDE累加
        for (int k = 0; k < 5; k++)
            h[k] += H[k];
    }   


    
    unsigned int *result = (unsigned int *)malloc(5 * sizeof(unsigned int));
    for (int i = 0; i < 5; ++i) {
        *(result + i) |= h[i];
    }
    std::cout << result << std::endl;
  return 0;
}