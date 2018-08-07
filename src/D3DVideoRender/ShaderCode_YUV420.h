/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	ע�⣺�ڱ���������Ϣ�ͳ������ӵ�ǰ���£����������⸴�ơ��޸ġ��������ļ���

*/


#ifndef YUV420_SHADER_CODE_H
#define YUV420_SHADER_CODE_H

const DWORD g_YUV420ps30_main[] =
{
    0xffff0300, 0x002ffffe, 0x42415443, 0x0000001c, 0x00000087, 0xffff0300, 
    0x00000003, 0x0000001c, 0x20008102, 0x00000080, 0x00000058, 0x00010003, 
    0x00000001, 0x00000060, 0x00000000, 0x00000070, 0x00020003, 0x00000001, 
    0x00000060, 0x00000000, 0x00000078, 0x00000003, 0x00000001, 0x00000060, 
    0x00000000, 0x78655455, 0x00657574, 0x000c0004, 0x00010001, 0x00000001, 
    0x00000000, 0x78655456, 0x00657574, 0x78655459, 0x00657574, 0x335f7370, 
    0x4d00305f, 0x6f726369, 0x74666f73, 0x29522820, 0x534c4820, 0x6853204c, 
    0x72656461, 0x6d6f4320, 0x656c6970, 0x2e392072, 0x392e3432, 0x322e3934, 
    0x00373033, 0x05000051, 0xa00f0000, 0x3f950a81, 0x3fcc4a9d, 0xbf5fcbb4, 
    0x00000000, 0x05000051, 0xa00f0001, 0x3f950a81, 0xbec89507, 0xbf501eac, 
    0x3f081b65, 0x05000051, 0xa00f0002, 0x3f950a81, 0x40011a54, 0xbf8af5f5, 
    0x00000000, 0x05000051, 0xa00f0003, 0x3f800000, 0x00000000, 0x00000000, 
    0x00000000, 0x0200001f, 0x80000005, 0x90030000, 0x0200001f, 0x80010005, 
    0x90030001, 0x0200001f, 0x80020005, 0x90030002, 0x0200001f, 0x90000000, 
    0xa00f0800, 0x0200001f, 0x90000000, 0xa00f0801, 0x0200001f, 0x90000000, 
    0xa00f0802, 0x03000042, 0x800f0000, 0x90e40002, 0xa0e40802, 0x02000001, 
    0x80040000, 0x80000000, 0x03000042, 0x800f0001, 0x90e40000, 0xa0e40800, 
    0x04000004, 0x80090000, 0x80000001, 0xa0640003, 0xa0250003, 0x03000008, 
    0x80010800, 0xa0e40000, 0x80f80000, 0x03000042, 0x800f0001, 0x90e40001, 
    0xa0e40801, 0x02000001, 0x80020000, 0x80000001, 0x03000009, 0x80020800, 
    0xa0e40001, 0x80e40000, 0x03000008, 0x80040800, 0xa0e40002, 0x80f40000, 
    0x02000001, 0x80080800, 0xa0000003, 0x0000ffff
};

const DWORD g_YUV420ps20_main[] =
{
    0xffff0200, 0x002ffffe, 0x42415443, 0x0000001c, 0x00000087, 0xffff0200, 
    0x00000003, 0x0000001c, 0x20008102, 0x00000080, 0x00000058, 0x00010003, 
    0x00000001, 0x00000060, 0x00000000, 0x00000070, 0x00020003, 0x00000001, 
    0x00000060, 0x00000000, 0x00000078, 0x00000003, 0x00000001, 0x00000060, 
    0x00000000, 0x78655455, 0x00657574, 0x000c0004, 0x00010001, 0x00000001, 
    0x00000000, 0x78655456, 0x00657574, 0x78655459, 0x00657574, 0x325f7370, 
    0x4d00305f, 0x6f726369, 0x74666f73, 0x29522820, 0x534c4820, 0x6853204c, 
    0x72656461, 0x6d6f4320, 0x656c6970, 0x2e392072, 0x392e3432, 0x322e3934, 
    0x00373033, 0x05000051, 0xa00f0000, 0x3f800000, 0x00000000, 0x00000000, 
    0x00000000, 0x05000051, 0xa00f0001, 0x3f950a81, 0x00000000, 0x3fcc4a9d, 
    0xbf5fcbb4, 0x05000051, 0xa00f0002, 0x3f950a81, 0xbec89507, 0xbf501eac, 
    0x3f081b65, 0x05000051, 0xa00f0003, 0x3f950a81, 0x40011a54, 0x00000000, 
    0xbf8af5f5, 0x0200001f, 0x80000000, 0xb0030000, 0x0200001f, 0x80000000, 
    0xb0030001, 0x0200001f, 0x80000000, 0xb0030002, 0x0200001f, 0x90000000, 
    0xa00f0800, 0x0200001f, 0x90000000, 0xa00f0801, 0x0200001f, 0x90000000, 
    0xa00f0802, 0x03000042, 0x800f0000, 0xb0e40000, 0xa0e40800, 0x03000042, 
    0x800f0001, 0xb0e40001, 0xa0e40801, 0x03000042, 0x800f0002, 0xb0e40002, 
    0xa0e40802, 0x02000001, 0x80080003, 0xa0000000, 0x02000001, 0x80020000, 
    0x80000001, 0x02000001, 0x80040000, 0x80000002, 0x02000001, 0x80080000, 
    0xa0000000, 0x03000009, 0x80010003, 0xa0e40001, 0x80e40000, 0x03000009, 
    0x80020003, 0xa0e40002, 0x80e40000, 0x03000009, 0x80040003, 0xa0e40003, 
    0x80e40000, 0x02000001, 0x800f0800, 0x80e40003, 0x0000ffff
};


#endif