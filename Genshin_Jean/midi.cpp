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
					if (0x90 <= mtTrack.event || mtTrack.event <= 0x9F) // 8X	�ɿ�����	1�ֽ������ţ�2�ֽ�����
						mKey.isDwon = true;
					if (0x80 <= mtTrack.event && mtTrack.event <= 0x8F) // 9X	��������	1�ֽ������ţ�2�ֽ�����
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
			// ʱ���= ��ʱȡ��λ * ��ʱȡ��λ
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
			case 0x00: // ���ù������ �����
				break;
			case 0x01: // ������ע -- �ı���Ϣ
				break;
			case 0x02: // ������Ȩ -- ��Ȩ��Ϣ
				printf_s("��Ȩ:%s", (char*)mTrack.data.data());
				break;
			case 0x03: // �������� -- �������⣺����ȫ�����죬��һ��ʹ�ñ�ʾ�����⣬�ڶ��α�ʾ������ �������� -- ������
				for (auto var : mTrack.data)
					printf_s("%c", var);
				printf_s("\n");
				break;
			case 0x04: // �������� -- �����ı���ͬ01/2��
				break;
			case 0x05: // ����ı�
				break;
			case 0x06: // ��� -- ���ı���ǣ�marker��
				break;
			case 0x07: // ��ʼ�� -- ���ı���¼��ʼ�㣨ͬ01/2��
				break;
			case 0x08: // Program name -- �����ļ�������
				break;
			case 0x09: // �豸���� DeviceName -- MIDI�豸������
				break;
			case 0x20: // MIDIͨ�� 01 MIDIͨ����0ͨ��Ϊ��һͨ��
				break;
			case 0x21: // MIDI�ӿ� 01 �ӿں���
				break;
			case 0x2F: // ���������־ 00 ��
				//n1 = ReadByte();
				ReadBytes(mrInfo.length - i);
				i = mrInfo.length;
				break;
			case 0x51: // �ٶ� 3�ֽ�������1��4��������΢����
				m_nQuarterNote = mTrack.data.at(0) << 16 | mTrack.data.at(1) << 8 | mTrack.data.at(2);
				m_nTickUnit = m_nQuarterNote / m_mtData.division;
				printf_s("�ٶ�:%d(us)\n", m_nQuarterNote); // ΢��
				break;
			case 0x54: // SMPTEʱ�� 05 SMPTE�Ŀ�ʼʱ�䣨ʱ���֣��룬֡����֡��
				break;
			case 0x58: // ���� 04 ���� ��ĸ��00(1), 01(2), 02(4), 03(8)��
				num = mTrack.data.at(0); // ���ӼǺŵķ���
				den = (int)pow(2, mTrack.data.at(1)); // ���ӼǺŷ�ĸ��־ , �����ֵ��2�Ĵ���ֵ, �統ǰ���õ�ֵ��2, �����ӼǺŵķ�ĸ�� 2^2 = 4, ��ĸ��4;
				printf_s("����:%d/%d\n", num, den);
				n1 = mTrack.data.at(2); // ÿ�� MIDI ʱ�Ӱ����� tick �� ; ��׼ֵ�� 24 , һ�㲻��ı� ;
				n2 = mTrack.data.at(3); // ÿ24��MIDIʱ��, ��Ӧ��32����������Ŀ;��׼ֵ��8,һ�㲻��ı� ;
				break;
			case 0x59: // ���� 02 ����������-7-1(����)��0��c��,17(����) ��С����0���������1��С����
				if ((mTrack.data.at(0) << 8 | mTrack.data.at(1)) == 0)
					printf_s("����:C\n");
				break;
			case 0x7F: // �����ض���Ϣ -- �����ض���Ϣ
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
