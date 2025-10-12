
#include <game/client/gameclient.h>
#include <game/mapitems.h>

class CNamePlatePartChillerCustomClient : public CNamePlatePartSprite
{
public:
	void Update(CGameClient &This, const CNamePlateData &Data) override
	{
		if(!g_Config.m_ClShowClientType || Data.m_ClientId == This.m_Snap.m_LocalClientId)
		{
			m_Visible = false;
			return;
		}

		if(This.m_aClients[Data.m_ClientId].m_CustomClient == CUSTOM_CLIENT_ID_KAIZO_NETWORK) //1 = Kaizo Network
		{
			m_Texture = g_pData->m_aImages[IMAGE_KZ_KAIZOICON].m_Id;
			m_Sprite = SPRITE_KZ_KAIZOICON;
			m_Visible = true;
		}
		else
		{
			m_Visible = false;
		}
		m_Color.a = Data.m_Color.a;
	}

	CNamePlatePartChillerCustomClient(CGameClient &This) :
		CNamePlatePartSprite(This)
		{
			m_Texture = g_pData->m_aImages[IMAGE_KZ_KAIZOICON].m_Id;
			m_Padding = vec2(0.0f, 0.0f);
			m_Sprite = SPRITE_KZ_KAIZOICON;
			m_Size = vec2(20 + DEFAULT_PADDING, 20 + DEFAULT_PADDING);
		}
};