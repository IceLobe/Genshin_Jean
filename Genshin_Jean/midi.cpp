#include "midi.h"

#define MIDI_MTHD MAKEFOURCC('M', 'T', 'h', 'd')
#define MIDI_MTRK MAKEFOURCC('M', 'T', 'r', 'k')

#define UN_MAKEFOURCC(num) ((DWORD)(num & 0xFF000000) >> 24 | ((DWORD)(num & 0xFF0000) >> 8) | ((DWORD)(num & 0xFF00) << 8) | ((DWORD)(num & 0xFF) << 24));
#define ARRY_MAKEFOURCC(arry) ((DWORD)(BYTE)(arry[0]) | ((DWORD)(BYTE)(arry[1]) << 8) | ((DWORD)(BYTE)(arry[2]) << 16) | ((DWORD)(BYTE)(arry[3]) << 24 ))
#define UNARRY_MAKEFOURCC(arry) ((DWORD)(BYTE)(arry[3]) | ((DWORD)(BYTE)(arry[2]) << 8) | ((DWORD)(BYTE)(arry[1]) << 16) | ((DWORD)(BYTE)(arry[0]) << 24 ))

#define MAKE_SHORT(n) (WORD)MAKEWORD(HIBYTE(n), LOBYTE(n))

bool midi::ReadMidiFile(const char* pChrMidiFile) {
	fopen_s(&fp, pChrMidiFile, "rb");
	if (fp) {
		m_mtData = ReadMThdData();
		std::vector<Track_Chunk> vecBytes = ReadMTrks(m_mtData.NumTracks);
		if (vecBytes.size() != (size_t)m_mtData.NumTracks)
			return false;
		int i = 0;
		m_vecMapKey.clear();
		for (size_t i = 0; i < vecBytes.size(); i++) {
			if (i == 0)
				continue;
			Track_Chunk track = vecBytes.at(i);
			std::vector<Mapping_Key> vecMapkey;
			for (size_t j = 0; j < track.track_event.size(); j++) {
				MTrk_Track mtTrack = track.track_event.at(j);
				Mapping_Key mKey = { mtTrack.type, (int)mtTrack.DeltaTime, false };
				if ((0x80 <= mtTrack.event) && (mtTrack.event <= 0x90)) {
					if (0x90 <= mtTrack.event || mtTrack.event <= 0x9F) // 8X	松开音符	1字节音符号，2字节力度
						mKey.isDwon = true;
					if (0x80 <= mtTrack.event && mtTrack.event <= 0x8F) // 9X	按下音符	1字节音符号，2字节力度
						mKey.isDwon = false;
					vecMapkey.push_back(mKey);
				}
			}
			m_vecMapKey.push_back(vecMapkey);
		}
		return true;
	}
	return false;
}

unsigned int midi::GetTickTime() {
	return m_nQuarterNote / m_nTickUnit;
}

int midi::ReadInt() {
	int num = 0;
	fread_s(&num, sizeof(int), sizeof(int), 1, fp);
	return num;
}

unsigned char midi::ReadByte() {
	unsigned char uChr = 0;
	fread_s(&uChr, sizeof(unsigned char), sizeof(unsigned char), 1, fp);
	return uChr;
}

std::vector<unsigned char> midi::ReadBytes(int Len) {
	std::vector<unsigned char> vecUChr;
	vecUChr.resize(Len, 0);
	fread_s(vecUChr.data(), Len * sizeof(unsigned char), sizeof(unsigned char), Len, fp);
	return vecUChr;
}

std::string midi::ReadString(int Len) {
	std::string sName;
	sName.resize(Len);
	fread_s((void*)sName.data(), Len * sizeof(char), sizeof(char), Len, fp);
	return sName;
}

Header_Chunk midi::ReadMThdData() {
	Header_Chunk mtData = {};
	fread_s(&mtData, sizeof(mtData), sizeof(mtData), 1, fp);
	if (ARRY_MAKEFOURCC(mtData.MThd) != MIDI_MTHD)
		return mtData;
	if (UNARRY_MAKEFOURCC(mtData.header_length) != 6)
		return mtData;
	mtData.format = MAKE_SHORT(mtData.format);
	mtData.NumTracks = MAKE_SHORT(mtData.NumTracks);
	mtData.division = MAKE_SHORT(mtData.division);
	return mtData;
}

Track_Chunk midi::ReadMTrkData() {
	int _ElementCount = 0;
	Track_Chunk mrInfo = {};
	mrInfo.MTrk = ReadInt();
	char chr[4] = {};
	memcpy_s(chr, 4, &mrInfo.MTrk, 4);
	if (mrInfo.MTrk != MIDI_MTRK)
		return mrInfo;
	mrInfo.length = ReadInt();
	mrInfo.length = UN_MAKEFOURCC(mrInfo.length);
	for (int i = 0; i < mrInfo.length;) {
		MTrk_Track mTrack = {};
		mTrack.v_time = ReadByte();
		++i;
		if (mTrack.v_time >= 0x80) {
			// 时间差= 计时取高位 * 计时取低位
			mTrack.DeltaTime = (mTrack.v_time & 0xF0) * (mTrack.v_time & 0xF);
			unsigned char uchr = ReadByte();
			++i;
			mTrack.DeltaTime += uchr;
			if (uchr >= 0x80) {
				uchr = ReadByte();
				++i;
				mTrack.DeltaTime += uchr;
			}
		}
		else {
			mTrack.DeltaTime = mTrack.v_time;
		}

		mTrack.event = ReadByte();
		++i;
		if ((mTrack.event & 0xF0) == 0xC0 || (mTrack.event & 0xF0) == 0xD0) {
			mTrack.type = ReadByte();
			++i;
		}
		else {
			mTrack.type = ReadByte();
			mTrack.LengthOrStrength = ReadByte();
			i += 2;
		}
		if (mTrack.event == 0xFF) {
			mTrack.data = ReadBytes(mTrack.LengthOrStrength);
			i += mTrack.LengthOrStrength;
			int num = 0, den = 0, n1 = 0, n2 = 0;
			switch (mTrack.type) {
			case 0x00: // 设置轨道音序 音序号
				break;
			case 0x01: // 歌曲备注 -- 文本信息
				break;
			case 0x02: // 歌曲版权 -- 版权信息
				printf_s("版权:%s", (char*)mTrack.data.data());
				break;
			case 0x03: // 歌曲标题 -- 歌曲标题：用于全局音轨，第一次使用表示主标题，第二次表示副标题 音轨名称 -- 音轨名
				for (auto var : mTrack.data)
					printf_s("%c", var);
				printf_s("\n");
				break;
			case 0x04: // 乐器名称 -- 音轨文本（同01/2）
				break;
			case 0x05: // 歌词文本
				break;
			case 0x06: // 标记 -- 用文本标记（marker）
				break;
			case 0x07: // 开始点 -- 用文本记录开始点（同01/2）
				break;
			case 0x08: // Program name -- 歌曲文件的名字
				break;
			case 0x09: // 设备名字 DeviceName -- MIDI设备的名字
				break;
			case 0x20: // MIDI通道 01 MIDI通道，0通常为第一通道
				break;
			case 0x21: // MIDI接口 01 接口号码
				break;
			case 0x2F: // 音轨结束标志 00 无
				//n1 = ReadByte();
				ReadBytes(mrInfo.length - i);
				i = mrInfo.length;
				break;
			case 0x51: // 速度 3字节整数，1个4分音符的微秒数
				m_nQuarterNote = mTrack.data.at(0) << 16 | mTrack.data.at(1) << 8 | mTrack.data.at(2);
				m_nTickUnit = m_nQuarterNote / m_mtData.division;
				printf_s("速度:%d(us)\n", m_nQuarterNote); // 微秒
				break;
			case 0x54: // SMPTE时间 05 SMPTE的开始时间（时，分，秒，帧，复帧）
				break;
			case 0x58: // 节拍 04 分子 分母：00(1), 01(2), 02(4), 03(8)等
				num = mTrack.data.at(0); // 拍子记号的分子
				den = (int)pow(2, mTrack.data.at(1)); // 拍子记号分母标志 , 这里的值是2的次幂值, 如当前设置的值是2, 则拍子记号的分母是 2^2 = 4, 分母是4;
				printf_s("节拍:%d/%d\n", num, den);
				n1 = mTrack.data.at(2); // 每个 MIDI 时钟包含的 tick 数 ; 标准值是 24 , 一般不会改变 ;
				n2 = mTrack.data.at(3); // 每24个MIDI时钟, 对应的32分音符的数目;标准值是8,一般不会改变 ;
				break;
			case 0x59: // 调号 02 升降号数：-7-1(降号)，0（c）,17(升号) 大小调：0（大调），1（小调）
				if ((mTrack.data.at(0) << 8 | mTrack.data.at(1)) == 0)
					printf_s("调号:C\n");
				break;
			case 0x7F: // 音符特定信息 -- 音符特定信息
				break;
			default:
				break;
			}
		}
		mrInfo.track_event.push_back(mTrack);
	}
	return mrInfo;
}

std::vector<Track_Chunk> midi::ReadMTrks(int NumTracks) {
	std::vector<Track_Chunk> vecBytes;
	for (int i = 0; i < NumTracks; i++) {
		vecBytes.push_back(ReadMTrkData());
	}
	return vecBytes;
}
