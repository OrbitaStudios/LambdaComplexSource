//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#include "shadersystem.h"
#include <stdlib.h>
#include "materialsystem_global.h"
#include "filesystem.h"
#include "tier1/utldict.h"
#include "shaderlib/ShaderDLL.h"
#include "texturemanager.h"
#include "itextureinternal.h"
#include "IHardwareConfigInternal.h"
#include "tier1/utlstack.h"
#include "tier1/utlbuffer.h"
#include "mathlib/vmatrix.h"
#include "imaterialinternal.h"
#include "tier1/strtools.h"
#include "tier0/icommandline.h"
#include "shaderlib/cshader.h"
#include "tier1/convar.h"
#include "tier1/keyvalues.h"
#include "shader_dll_verify.h"
#include "tier0/vprof.h"
#include "tier1/tier1_logging.h"

// NOTE: This must be the last file included!
#include "tier0/memdbgon.h"

#if defined( _PS3 ) || defined( _OSX )
#define g_pShaderAPI ShaderAPI()
#define ShaderApiParam( x ) g_pShaderAPIDX8
#else
#define ShaderApiParam( x ) x
#endif


//#define DEBUG_DEPTH 1

//-----------------------------------------------------------------------------
// Lovely convars
//-----------------------------------------------------------------------------
static ConVar mat_showenvmapmask( "mat_showenvmapmask", "0" );
static ConVar mat_debugdepth( "mat_debugdepth", "0" );
extern ConVar mat_supportflashlight;


//-----------------------------------------------------------------------------
// Implementation of the shader system
//-----------------------------------------------------------------------------
class CShaderSystem : public IShaderSystemInternal
{
public:
	CShaderSystem();

	// Methods of IShaderSystem
	virtual ShaderAPITextureHandle_t GetShaderAPITextureBindHandle( ITexture *pTexture, int nFrameVar, int nTextureChannel = 0 );

	virtual void		BindTexture( Sampler_t sampler1, TextureBindFlags_t nBindFlags, ITexture *pTexture, int nFrame = 0 );
	virtual void		BindTexture( Sampler_t sampler1, Sampler_t sampler2, TextureBindFlags_t nBindFlags, ITexture *pTexture, int nFrame = 0 );
	virtual void		BindVertexTexture( VertexTextureSampler_t vtSampler, ITexture *pTexture, int nFrame = 0 );

	virtual void		TakeSnapshot( );
	virtual void		DrawSnapshot( const unsigned char *pInstanceCommandBuffer, bool bMakeActualDrawCall = true );
	virtual bool		IsUsingGraphics() const;

	// Methods of IShaderSystemInternal
	virtual void		Init();
	virtual void		Shutdown();
	virtual void		ModInit();
	virtual void		ModShutdown();

	virtual bool		LoadShaderDLL( const char *pFullPath );
	virtual bool		LoadShaderDLL( const char *pFullPath, const char *pPathID, bool bModShaderDLL );
	virtual void		UnloadShaderDLL( const char *pFullPath );

	virtual IShader*	FindShader( char const* pShaderName );
	virtual void		CreateDebugMaterials();
	virtual void		CleanUpDebugMaterials();
	virtual char const* ShaderStateString( int i ) const;
	virtual int			ShaderStateCount( ) const;

	virtual void		InitShaderParameters( IShader *pShader, IMaterialVar **params, const char *pMaterialName );
	virtual void		InitShaderInstance( IShader *pShader, IMaterialVar **params, const char *pMaterialName, const char *pTextureGroupName );
	virtual bool		InitRenderState( IShader *pShader, int numParams, IMaterialVar **params, ShaderRenderState_t* pRenderState, char const* pMaterialName );
	virtual void		CleanupRenderState( ShaderRenderState_t* pRenderState );
	virtual void		DrawElements( IShader *pShader, IMaterialVar **params, ShaderRenderState_t* pShaderState, VertexCompressionType_t vertexCompression, uint32 nVarChangeID, uint32 nModulationFlags, bool bRenderingPreTessPatchMesh );

	// Used to iterate over all shaders for editing purposes
	virtual int			ShaderCount() const;
	virtual int			GetShaders( int nFirstShader, int nMaxCount, IShader **ppShaderList ) const;

	// Methods of IShaderInit
	virtual void		LoadTexture( IMaterialVar *pTextureVar, const char *pTextureGroupName, int nAdditionalCreationFlags = 0 );
	virtual void		LoadBumpMap( IMaterialVar *pTextureVar, const char *pTextureGroupName, int nAdditionalCreationFlags = 0 );
	virtual void		LoadCubeMap( IMaterialVar **ppParams, IMaterialVar *pTextureVar, int nAdditionalCreationFlags = 0 );

	bool CanUseEditorMaterials() const;

	virtual void AddShaderComboInformation( const ShaderComboSemantics_t *pSemantics );

private:
	struct ShaderDLLInfo_t
	{
		char *m_pFileName;
		CSysModule *m_hInstance;
		IShaderDLLInternal *m_pShaderDLL;
		ShaderDLL_t m_hShaderDLL;
		
		// True if this is a mod's shader DLL, in which case it's not allowed to 
		// override any existing shader names.
		bool m_bModShaderDLL;
		CUtlDict< IShader *, unsigned short >	m_ShaderDict; 
	};

private:
	// Load up the shader DLLs...
	void LoadAllShaderDLLs();

	// Load the "mshader_" DLLs.
	void LoadModShaderDLLs( int dxSupportLevel );

	// Unload all the shader DLLs...
	void UnloadAllShaderDLLs();

	// Sets up the shader dictionary.
	void SetupShaderDictionary( int nShaderDLLIndex );

	// Cleans up the shader dictionary.
	void CleanupShaderDictionary( int nShaderDLLIndex );

	// Finds an already loaded shader DLL
	int FindShaderDLL( const char *pFullPath );

	// Unloads a particular shader DLL
	void UnloadShaderDLL( int nShaderDLLIndex );

	// Sets up the current ShaderState_t for rendering
	void PrepForShaderDraw( IShader *pShader, IMaterialVar** ppParams, 
		ShaderRenderState_t* pRenderState, int modulation );
	void DoneWithShaderDraw();

	// Initializes state snapshots
	void InitStateSnapshots( IShader *pShader, IMaterialVar **params, ShaderRenderState_t* pRenderState );

	// Compute snapshots for all combinations of alpha + color modulation
	void InitRenderStateFlags( ShaderRenderState_t* pRenderState, int numParams, IMaterialVar **params );

	// Computes flags from a particular snapshot
	void ComputeRenderStateFlagsFromSnapshot( ShaderRenderState_t* pRenderState );

	// Computes vertex format + usage from a particular snapshot
	bool ComputeVertexFormatFromSnapshot( IMaterialVar **params, ShaderRenderState_t* pRenderState );

	// Gets at the current snapshot
	StateSnapshot_t CurrentStateSnapshot();

	// Draws using a particular material..
	void DrawUsingMaterial( IMaterialInternal *pMaterial, VertexCompressionType_t vertexCompression );

	// Copies material vars
	void CopyMaterialVarToDebugShader( IMaterialInternal *pDebugMaterial, IShader *pShader, IMaterialVar **ppParams, const char *pSrcVarName, const char *pDstVarName = NULL );

	// Debugging draw methods...
	void DrawMeasureFillRate( ShaderRenderState_t* pRenderState, int mod, VertexCompressionType_t vertexCompression );
	void DrawNormalMap( IShader *pShader, IMaterialVar **ppParams, VertexCompressionType_t vertexCompression );
	bool DrawEnvmapMask( IShader *pShader, IMaterialVar **ppParams, ShaderRenderState_t* pRenderState, VertexCompressionType_t vertexCompression );

	int GetModulationSnapshotCount( IMaterialVar **params );

private:
	// List of all DLLs containing shaders
	CUtlVector< ShaderDLLInfo_t > m_ShaderDLLs;

	// Used to prevent re-entrant rendering from warning messages.
	CBufferedLoggingListener m_BufferedLoggingListener;
	// Causes Log_Error() to break into debugger instead of exiting.
	CNonFatalLoggingResponsePolicy m_NonFatalLoggingResponsePolicy;

	// Render state we're drawing with
	ShaderRenderState_t* m_pRenderState;
	unsigned short m_hShaderDLL;
	unsigned char m_nModulation;
	unsigned char m_nRenderPass;

	// Debugging materials
	// If you add to this, add to the list of debug shader names (s_pDebugShaderName) below
	enum
	{
		MATERIAL_FILL_RATE = 0,
		MATERIAL_DEBUG_NORMALMAP,
		MATERIAL_DEBUG_ENVMAPMASK,
		MATERIAL_DEBUG_DEPTH,
		MATERIAL_DEBUG_DEPTH_DECAL,
		MATERIAL_DEBUG_WIREFRAME,

		MATERIAL_DEBUG_COUNT,
	};

	IMaterialInternal* m_pDebugMaterials[MATERIAL_DEBUG_COUNT];
	static const char *s_pDebugShaderName[MATERIAL_DEBUG_COUNT];

	bool			m_bForceUsingGraphicsReturnTrue;
};


//-----------------------------------------------------------------------------
// Singleton
//-----------------------------------------------------------------------------
static CShaderSystem s_ShaderSystem;
IShaderSystemInternal *g_pShaderSystem = &s_ShaderSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CShaderSystem, IShaderSystem, 
						SHADERSYSTEM_INTERFACE_VERSION, s_ShaderSystem );


//-----------------------------------------------------------------------------
// Debugging shader names
//-----------------------------------------------------------------------------
const char *CShaderSystem::s_pDebugShaderName[MATERIAL_DEBUG_COUNT]	=
{
	"FillRate",
	"DebugNormalMap",
	"DebugDrawEnvmapMask",
	"DebugDepth",
	"DebugDepth",
	"Wireframe_DX9"
};

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CShaderSystem::CShaderSystem() : m_bForceUsingGraphicsReturnTrue( false )
{
}


//-----------------------------------------------------------------------------
// Initialization, shutdown
//-----------------------------------------------------------------------------
void CShaderSystem::Init()
{
	m_bForceUsingGraphicsReturnTrue = false;
	if ( CommandLine()->FindParm( "-noshaderapi" ) ||
		 CommandLine()->FindParm( "-makereslists" ) )
	{
		m_bForceUsingGraphicsReturnTrue = true;
	}

	for ( int i = 0; i < MATERIAL_DEBUG_COUNT; ++i )
	{
		m_pDebugMaterials[i] = NULL;
	}

	LoadAllShaderDLLs();
}

void CShaderSystem::Shutdown()
{
	UnloadAllShaderDLLs();
}


//-----------------------------------------------------------------------------
// Load/unload mod-specific shader DLLs
//-----------------------------------------------------------------------------
void CShaderSystem::ModInit()
{
	// Load up standard shader DLLs...
	int dxSupportLevel = HardwareConfig()->GetMaxDXSupportLevel();
	Assert( dxSupportLevel >= 60 );
	dxSupportLevel /= 10;

	LoadModShaderDLLs( dxSupportLevel );
}


void CShaderSystem::ModShutdown()
{
	// Unload only MOD dlls
	for ( int i = m_ShaderDLLs.Count(); --i >= 0; )
	{
		if ( m_ShaderDLLs[i].m_bModShaderDLL )
		{
			UnloadShaderDLL(i);
			delete[] m_ShaderDLLs[i].m_pFileName;
			m_ShaderDLLs.Remove( i );
		}
	}
}


//-----------------------------------------------------------------------------
// Load up the shader DLLs...
//-----------------------------------------------------------------------------
void CShaderSystem::LoadAllShaderDLLs( )
{
	UnloadAllShaderDLLs();

	GetShaderDLLInternal()->Connect( Sys_GetFactoryThis(), true );

	// Loads local defined or statically linked shaders
	int i = m_ShaderDLLs.AddToHead();

	m_ShaderDLLs[i].m_pFileName     = new char[1];
	m_ShaderDLLs[i].m_pFileName[0]  = 0;
	m_ShaderDLLs[i].m_hInstance     = NULL;
	m_ShaderDLLs[i].m_pShaderDLL    = GetShaderDLLInternal();
	m_ShaderDLLs[i].m_bModShaderDLL = false;

	// Add the shaders to the dictionary of shaders...
	SetupShaderDictionary( i );

#if defined( _PS3 ) || defined( _OSX )
	LoadShaderDLL( "stdshader_dx9" DLL_EXT_STRING );
#else // _PS3 || _OSX

	// 360 has the the debug shaders in its dx9 dll
	if ( IsPC() || !IsX360() )
	{
		// Always need the debug shaders
		LoadShaderDLL( "stdshader_dbg" );
	}

	// Load up standard shader DLLs...
	int dxSupportLevel = HardwareConfig()->GetMaxDXSupportLevel();
	Assert( dxSupportLevel >= 60 );
	dxSupportLevel /= 10;

	// 360 only supports its dx9 dll
	int dxStart = 9;
	char buf[32];
	for ( i = dxStart; i <= dxSupportLevel; ++i )
	{
		Q_snprintf( buf, sizeof( buf ), "stdshader_dx%d", i );
		LoadShaderDLL( buf );
	}

	const char *pShaderName = NULL;
#ifdef _DEBUG
	pShaderName = CommandLine()->ParmValue( "-shader" );
#endif
	if ( !pShaderName )
	{
		pShaderName = HardwareConfig()->GetHWSpecificShaderDLLName();
	}
	if ( pShaderName )
	{
		LoadShaderDLL( pShaderName );
	}

#ifdef _DEBUG
	// For fast-iteration debugging
	if ( CommandLine()->FindParm( "-testshaders" ) )
	{
		LoadShaderDLL( "shader_test" );
	}
#endif
#endif // !_PS3
}

void CShaderSystem::LoadModShaderDLLs( int dxSupportLevel )
{
	// @wge: Not so sure about this OSX addition, may break modding support!
	return;	// no more support for custom game shaders to control which DLLs we allow loading

	const char *pModShaderPathID = "GAMEBIN";

	// First load the ones with dx_ prefix.
	char buf[256];

	int dxStart = 6;
	for ( int i = dxStart; i <= dxSupportLevel; ++i )
	{
		Q_snprintf( buf, sizeof( buf ), "game_shader_dx%d", i );
		LoadShaderDLL( buf, pModShaderPathID, true );
	}

	// Now load the ones with any dx_ prefix.
	FileFindHandle_t findHandle;
	const char *pFilename = g_pFullFileSystem->FindFirstEx( "game_shader_generic*", pModShaderPathID, &findHandle );
	while ( pFilename )
	{
		Q_snprintf( buf, sizeof( buf ), "%s", pFilename );
		LoadShaderDLL( buf, pModShaderPathID, true );

		pFilename = g_pFullFileSystem->FindNext( findHandle );
	}
}


//-----------------------------------------------------------------------------
// Unload all the shader DLLs...
//-----------------------------------------------------------------------------
void CShaderSystem::UnloadAllShaderDLLs()
{
	if ( m_ShaderDLLs.Count() == 0 )
		return;

	for ( int i = m_ShaderDLLs.Count(); --i >= 0; )
	{
		UnloadShaderDLL(i);
		delete[] m_ShaderDLLs[i].m_pFileName;
	}

	m_ShaderDLLs.RemoveAll();
}

bool CShaderSystem::LoadShaderDLL( const char *pFullPath )
{
	return LoadShaderDLL( pFullPath, NULL, false );
}

//-----------------------------------------------------------------------------
// Methods related to reading in shader DLLs
//-----------------------------------------------------------------------------
bool CShaderSystem::LoadShaderDLL( const char *pFullPath, const char *pPathID, bool bModShaderDLL )
{
#if !defined( _PS3 ) && !defined( _OSX )
	if ( !pFullPath && !pFullPath[0] )
		return true;

	// Load the new shader
	bool bValidatedDllOnly = true;
	if ( bModShaderDLL )
		bValidatedDllOnly = false;

	CSysModule *hInstance = g_pFullFileSystem->LoadModule( pFullPath, pPathID, bValidatedDllOnly );
	if ( !hInstance )
		return false;

	// Get at the shader DLL interface
	CreateInterfaceFn factory = Sys_GetFactory( hInstance );
	if (!factory)
	{
		g_pFullFileSystem->UnloadModule( hInstance );
		return false;
	}

	IShaderDLLInternal *pShaderDLL = (IShaderDLLInternal*)factory( SHADER_DLL_INTERFACE_VERSION, NULL );
	if ( !pShaderDLL )
	{
		g_pFullFileSystem->UnloadModule( hInstance );
		return false;
	}

	// Allow the DLL to try to connect to interfaces it needs
	if ( !pShaderDLL->Connect( Sys_GetFactoryThis(), false ) )
	{
		g_pFullFileSystem->UnloadModule( hInstance );
		return false;
	}

#else

	CSysModule *hInstance = NULL;
	IShaderDLLInternal *pShaderDLL = GetShaderDLLInternal();
	pShaderDLL->Connect( Sys_GetFactoryThis(), false );

#endif // !_PS3 && !_OSX

	// FIXME: We need to do some sort of shader validation here for anticheat.

	// Now replace any existing shader
	int nShaderDLLIndex = FindShaderDLL( pFullPath );
	if ( nShaderDLLIndex >= 0 )
	{
		UnloadShaderDLL( nShaderDLLIndex );
	}
	else
	{
		nShaderDLLIndex = m_ShaderDLLs.AddToTail();
		int nLen = Q_strlen(pFullPath) + 1;
		m_ShaderDLLs[nShaderDLLIndex].m_pFileName = new char[ nLen ];
		Q_strncpy( m_ShaderDLLs[nShaderDLLIndex].m_pFileName, pFullPath, nLen );
	}

	// Ok, the shader DLL's good!
	m_ShaderDLLs[nShaderDLLIndex].m_hInstance = hInstance;
	m_ShaderDLLs[nShaderDLLIndex].m_pShaderDLL = pShaderDLL;
	m_ShaderDLLs[nShaderDLLIndex].m_bModShaderDLL = bModShaderDLL;
	
	// Add the shaders to the dictionary of shaders...
	SetupShaderDictionary( nShaderDLLIndex );
	
	// FIXME: Fix up existing materials that were using shaders that have
	// been reloaded?

	return true;
}

//-----------------------------------------------------------------------------
// Finds an already loaded shader DLL
//-----------------------------------------------------------------------------
int CShaderSystem::FindShaderDLL( const char *pFullPath )
{
	for ( int i = m_ShaderDLLs.Count(); --i >= 0; )
	{
		if ( !Q_stricmp( pFullPath, m_ShaderDLLs[i].m_pFileName ) )
			return i;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Unloads a particular shader DLL
//-----------------------------------------------------------------------------
void CShaderSystem::UnloadShaderDLL( int nShaderDLLIndex )
{
	if ( nShaderDLLIndex < 0 )
		return;

	// FIXME: Do some sort of fixup of materials to determine which
	// materials are referencing shaders in this DLL?
	CleanupShaderDictionary( nShaderDLLIndex );
	IShaderDLLInternal *pShaderDLL = m_ShaderDLLs[nShaderDLLIndex].m_pShaderDLL;
	pShaderDLL->Disconnect( pShaderDLL == GetShaderDLLInternal() );
	if ( m_ShaderDLLs[nShaderDLLIndex].m_hInstance )
	{
		g_pFullFileSystem->UnloadModule( m_ShaderDLLs[nShaderDLLIndex].m_hInstance );
	}
}

//-----------------------------------------------------------------------------
// Unloads a particular shader DLL
//-----------------------------------------------------------------------------
void CShaderSystem::UnloadShaderDLL( const char *pFullPath )
{
	int nShaderDLLIndex = FindShaderDLL( pFullPath );
	if ( nShaderDLLIndex >= 0 )
	{
		UnloadShaderDLL( nShaderDLLIndex );
		delete[] m_ShaderDLLs[nShaderDLLIndex].m_pFileName;
		m_ShaderDLLs.Remove( nShaderDLLIndex ); 
	}
}


//-----------------------------------------------------------------------------
// Make sure these match the bits in imaterial.h
//-----------------------------------------------------------------------------
static char* s_pShaderStateString[] =
{
	"$debug",
	"$no_fullbright",
	"$no_draw",
	"$use_in_fillrate_mode",

	"$vertexcolor",
	"$vertexalpha",
	"$selfillum",
	"$additive",
	"$alphatest",
	"$pseudotranslucent",
	"$znearer",
	"$model",
	"$flat",
	"$nocull",
	"$nofog",
	"$ignorez",
	"$decal",
	"$envmapsphere",
	"$xxxxxxunusedxxxxx",
	"$envmapcameraspace",
	"$basealphaenvmapmask",
	"$translucent",
	"$normalmapalphaenvmapmask",
	"$softwareskin",
	"$opaquetexture",
	"$multiply",
	"$nodecal",
	"$halflambert",
	"$wireframe",
	"$allowalphatocoverage",
	"$alphamodifiedbyproxy_DO_NOT_SET_IN_VMT", // This one is only used as a flag from code externally.  Isnt' to be st in vmt files.
	"$vertexfog",

	""			// last one must be null
};


//-----------------------------------------------------------------------------
// returns strings associated with the shader state flags...
// If you modify this, make sure and modify MaterialVarFlags_t in imaterial.h
//-----------------------------------------------------------------------------
int CShaderSystem::ShaderStateCount( ) const
{
	return sizeof( s_pShaderStateString ) / sizeof( char* ) - 1;
}


//-----------------------------------------------------------------------------
// returns strings associated with the shader state flags...
// If you modify this, make sure and modify MaterialVarFlags_t in imaterial.h
//-----------------------------------------------------------------------------
char const* CShaderSystem::ShaderStateString( int i ) const
{
	return s_pShaderStateString[i];
}


//-----------------------------------------------------------------------------
// Sets up the shader dictionary.
//-----------------------------------------------------------------------------
void CShaderSystem::SetupShaderDictionary( int nShaderDLLIndex )
{
	// We could have put the shader dictionary into each shader DLL
	// I'm not sure if that makes this system any less secure than it already is
	ShaderDLLInfo_t &info = m_ShaderDLLs[nShaderDLLIndex];

	for ( int i = 0; i < info.m_pShaderDLL->ShaderComboSemanticsCount(); i++ )
	{
		const ShaderComboSemantics_t *pSemantics = info.m_pShaderDLL->GetComboSemantics( i );
		g_pShaderAPI->AddShaderComboInformation( pSemantics );
	}


	int nCount = info.m_pShaderDLL->ShaderCount();
	for ( int i = 0; i < nCount; ++i )
	{
		IShader *pShader = info.m_pShaderDLL->GetShader( i );
		const char *pShaderName = pShader->GetName();

#ifdef POSIX
		//if (CommandLine()->FindParm("-glmspew"))
		//	printf("CShaderSystem::SetupShaderDictionary: %s", pShaderName );
#endif
		
		// Make sure it doesn't try to override another shader DLL's names.
		if ( info.m_bModShaderDLL )
		{
			for ( int iTestDLL=0; iTestDLL < m_ShaderDLLs.Count(); iTestDLL++ )
			{
				ShaderDLLInfo_t *pTestDLL = &m_ShaderDLLs[iTestDLL];
				if ( !pTestDLL->m_bModShaderDLL )
				{
					if ( pTestDLL->m_ShaderDict.Find( pShaderName ) != pTestDLL->m_ShaderDict.InvalidIndex() )
					{ 
						Error( "Game shader '%s' trying to override a base shader '%s'.", info.m_pFileName, pShaderName );
					}
				}
			}
		}

		info.m_ShaderDict.Insert( pShaderName, pShader );
	}
}

//-----------------------------------------------------------------------------
// Cleans up the shader dictionary.
//-----------------------------------------------------------------------------
void CShaderSystem::CleanupShaderDictionary( int nShaderDLLIndex )
{
}

//-----------------------------------------------------------------------------
// Finds a shader in the shader dictionary
//-----------------------------------------------------------------------------
IShader* CShaderSystem::FindShader( char const* pShaderName )
{
	// FIXME: What kind of search order should we use here?
	// I'm currently assuming last added, first searched.
	for (int i = m_ShaderDLLs.Count(); --i >= 0; )
	{
		ShaderDLLInfo_t &info = m_ShaderDLLs[i];
		unsigned short idx = info.m_ShaderDict.Find( pShaderName );
		if ( idx != info.m_ShaderDict.InvalidIndex() )
		{
			return info.m_ShaderDict[idx];
		}
	}

	return NULL;
}


//-----------------------------------------------------------------------------
// Used to iterate over all shaders for editing purposes
//-----------------------------------------------------------------------------
int CShaderSystem::ShaderCount() const
{
	return GetShaders( 0, 65536, NULL );
}

int CShaderSystem::GetShaders( int nFirstShader, int nMaxCount, IShader **ppShaderList ) const
{
	CUtlSymbolTable	uniqueNames( 0, 512, true ); 

	int nCount = 0;
	int nActualCount = 0;
	for ( int i = m_ShaderDLLs.Count(); --i >= 0; )
	{
		const ShaderDLLInfo_t &info = m_ShaderDLLs[i];
		for ( unsigned short j = info.m_ShaderDict.First(); 
			j != info.m_ShaderDict.InvalidIndex();
			j = info.m_ShaderDict.Next( j ) )
		{
			// Don't add shaders twice
			const char *pShaderName = info.m_ShaderDict.GetElementName( j );
			if ( uniqueNames.Find( pShaderName ).IsValid() )
				continue;

			// Indicate we've seen this shader
			uniqueNames.AddString( pShaderName );

			++nActualCount;
			if ( nActualCount > nFirstShader )
			{
				if ( ppShaderList )
				{
					ppShaderList[ nCount ] = info.m_ShaderDict[j];
				}
				++nCount;
				if ( nCount >= nMaxCount )
					return nCount;
			}
		}
	}

	return nCount;
}

	
//-----------------------------------------------------------------------------
//
// Methods of IShaderInit lie below
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Gets at the render pass info for this pass...
//-----------------------------------------------------------------------------
inline StateSnapshot_t CShaderSystem::CurrentStateSnapshot()
{
	Assert( m_pRenderState );
	Assert( m_nRenderPass < MAX_RENDER_PASSES );
	Assert( m_nRenderPass < m_pRenderState->m_pSnapshots[m_nModulation].m_nPassCount );
	return m_pRenderState->m_pSnapshots[m_nModulation].m_Snapshot[m_nRenderPass];
}


//-----------------------------------------------------------------------------
// Create debugging materials
//-----------------------------------------------------------------------------
void CShaderSystem::CreateDebugMaterials()
{
	if (m_pDebugMaterials[0])
		return;

	KeyValues *pVMTKeyValues[MATERIAL_DEBUG_COUNT];

	int i;
	for ( i = 0; i < MATERIAL_DEBUG_COUNT; ++i )
	{
		pVMTKeyValues[i] = new KeyValues( s_pDebugShaderName[i] );
	}
	
	pVMTKeyValues[MATERIAL_DEBUG_DEPTH_DECAL]->SetInt( "$decal", 1 );

	for ( i = 0; i < MATERIAL_DEBUG_COUNT; ++i )
	{
		char shaderName[64];
		Q_snprintf( shaderName, sizeof( shaderName ), "___%s_%d.vmt", s_pDebugShaderName[i], i );
		m_pDebugMaterials[i] = static_cast<IMaterialInternal*>(MaterialSystem()->CreateMaterial( shaderName, pVMTKeyValues[i] ));
		if( m_pDebugMaterials[i] )
			m_pDebugMaterials[i] = m_pDebugMaterials[i]->GetRealTimeVersion();
	}
}


//-----------------------------------------------------------------------------
// Cleans up the debugging materials
//-----------------------------------------------------------------------------
void CShaderSystem::CleanUpDebugMaterials()
{
	if (m_pDebugMaterials[0])
	{
		for ( int i = 0; i < MATERIAL_DEBUG_COUNT; ++i )
		{
			m_pDebugMaterials[i]->DecrementReferenceCount();
			if ( m_pDebugMaterials[i]->InMaterialPage() )
			{
				MaterialSystem()->RemoveMaterialSubRect( m_pDebugMaterials[i] );
			}
			else
			{
				MaterialSystem()->RemoveMaterial( m_pDebugMaterials[i] );
			}
			m_pDebugMaterials[i] = NULL;
		}
	}
}

//-----------------------------------------------------------------------------
// Deals with shader draw
//-----------------------------------------------------------------------------
void CShaderSystem::PrepForShaderDraw( IShader *pShader,
	IMaterialVar** ppParams, ShaderRenderState_t* pRenderState, int nModulation )
{
	Assert( !m_pRenderState );

	// 360 runs the console remotely, spew cannot cause the matsys to be reentrant
	// 360 sidesteps the other negative affect that *all* buffered spew redirects as warning text
	if ( IsPC() || ( !IsX360() && !IsPS3() ) )
	{
		LoggingSystem_PushLoggingState( true );
		LoggingSystem_RegisterLoggingListener( &m_BufferedLoggingListener );
		LoggingSystem_SetLoggingResponsePolicy( &m_NonFatalLoggingResponsePolicy );
	}

	m_pRenderState = pRenderState;
	m_nModulation = nModulation;
	m_nRenderPass = 0;
}

void CShaderSystem::DoneWithShaderDraw()
{
	if ( IsPC() || ( !IsX360() && !IsPS3() ) )
	{
		LoggingSystem_PopLoggingState( true );
		m_BufferedLoggingListener.EmitBufferedSpew();
	}

	m_pRenderState = NULL;
}


#ifdef _DEBUG
#pragma warning (disable:4189)
#endif

//-----------------------------------------------------------------------------
// Call the SHADER_PARAM_INIT block of the shaders
//-----------------------------------------------------------------------------
void CShaderSystem::InitShaderParameters( IShader *pShader, IMaterialVar **params, const char *pMaterialName )
{
#ifdef _DEBUG
	if ( IsFlagSet( params, MATERIAL_VAR_DEBUG ) )
	{
		// Putcher breakpoint here to catch the rendering of a material
		// marked for debugging ($debug = 1 in a .vmt file) init params version
		int x = 0;
	}
#endif

	// Let the derived class do its thing
	PrepForShaderDraw( pShader, params, 0, 0 );
	pShader->InitShaderParams( params, pMaterialName );
	DoneWithShaderDraw();

	// Set up color + alpha defaults
	if (!params[COLOR]->IsDefined())
	{
		params[COLOR]->SetVecValue( 1.0f, 1.0f, 1.0f );
	}

	if (!params[ALPHA]->IsDefined())
	{
		params[ALPHA]->SetFloatValue( 1.0f );
	}

	// Initialize all shader params based on their type...
	int i;
	for ( i = pShader->GetParamCount(); --i >= 0; )
	{
		// Don't initialize parameters that are already set up
		if (params[i]->IsDefined())
			continue;

		int type = pShader->GetParamInfo( i ).m_Type;
		switch( type )
		{
		case SHADER_PARAM_TYPE_TEXTURE:
			// Do nothing; we'll be loading in a string later
			break;
		case SHADER_PARAM_TYPE_STRING:
			// Do nothing; we'll be loading in a string later
			break;
		case SHADER_PARAM_TYPE_MATERIAL:
			params[i]->SetMaterialValue( NULL );
			break;
		case SHADER_PARAM_TYPE_BOOL:
		case SHADER_PARAM_TYPE_INTEGER:
			params[i]->SetIntValue( 0 );
			break;
		case SHADER_PARAM_TYPE_COLOR:
			params[i]->SetVecValue( 1.0f, 1.0f, 1.0f );
			break;
		case SHADER_PARAM_TYPE_VEC2:
			params[i]->SetVecValue( 0.0f, 0.0f );
			break;
		case SHADER_PARAM_TYPE_VEC3:
			params[i]->SetVecValue( 0.0f, 0.0f, 0.0f );
			break;
		case SHADER_PARAM_TYPE_VEC4:
			params[i]->SetVecValue( 0.0f, 0.0f, 0.0f, 0.0f );
			break;
		case SHADER_PARAM_TYPE_FLOAT:
			params[i]->SetFloatValue( 0 );
			break;
		case SHADER_PARAM_TYPE_FOURCC:
			params[i]->SetFourCCValue( 0, 0 );
			break;
		case SHADER_PARAM_TYPE_MATRIX:
			{
				VMatrix identity;
				MatrixSetIdentity( identity );
				params[i]->SetMatrixValue( identity );
			}
			break;

		default:
			Assert(0);
		}
	}
}

#ifdef _DEBUG
#pragma warning (default:4189)
#endif

//-----------------------------------------------------------------------------
// Call the SHADER_INIT block of the shaders
//-----------------------------------------------------------------------------
void CShaderSystem::InitShaderInstance( IShader *pShader, IMaterialVar **params, const char *pMaterialName, const char *pTextureGroupName )
{
	// Let the derived class do its thing
	PrepForShaderDraw( pShader, params, 0, 0 );
	pShader->InitShaderInstance( params, ShaderSystem(), pMaterialName, pTextureGroupName );
	DoneWithShaderDraw();
}


//-----------------------------------------------------------------------------
// Compute snapshots for all combinations of alpha + color modulation
//-----------------------------------------------------------------------------
void CShaderSystem::InitRenderStateFlags( ShaderRenderState_t* pRenderState, int numParams, IMaterialVar **params )
{
	// Compute vertex format and flags
	pRenderState->m_Flags = 0;

	// Make sure the shader don't force these flags. . they are automatically computed.
	Assert( !( pRenderState->m_Flags & SHADER_OPACITY_TRANSLUCENT ) );
	Assert( !( pRenderState->m_Flags & SHADER_OPACITY_ALPHATEST ) );
	Assert( !( pRenderState->m_Flags & SHADER_OPACITY_OPAQUE ) );

	// If we are in release mode, just go ahead and clear in case the above is screwed up.
	pRenderState->m_Flags &= ~SHADER_OPACITY_MASK;

/*
	// HACK: Also kind of gross; turn off bump lightmapping for low-end
	if (g_config.bUseGraphics && !HardwareConfig()->SupportsVertexAndPixelShaders())
	{
		pRenderState->m_Flags &= ~SHADER_NEEDS_BUMPED_LIGHTMAPS;
	}
*/
/*
	// HACK: more grossness!!!  turn off bump lightmapping if we don't have a bumpmap
	// Shaders should specify SHADER_NEEDS_BUMPED_LIGHTMAPS if they might need a bumpmap,
	// and this'll take care of getting rid of it if it isn't there.
	if( pRenderState->m_Flags & SHADER_NEEDS_BUMPED_LIGHTMAPS )
	{
		pRenderState->m_Flags &= ~SHADER_NEEDS_BUMPED_LIGHTMAPS;
		for( int i = 0; i < numParams; i++ )
		{
			if( stricmp( params[i]->GetName(), "$bumpmap" ) == 0 )
			{
				if( params[i]->IsDefined() )
				{
					const char *blah = params[i]->GetStringValue();
					pRenderState->m_Flags |= SHADER_NEEDS_BUMPED_LIGHTMAPS;
					break;
				}
			}
		}
	}
*/
}


//-----------------------------------------------------------------------------
// Computes flags from a particular snapshot
//-----------------------------------------------------------------------------
void CShaderSystem::ComputeRenderStateFlagsFromSnapshot( ShaderRenderState_t* pRenderState )
{
	// When computing the flags, use the snapshot that has no alpha or color
	// modulation. When asking for translucency, we'll have to check for
	// alpha modulation in addition to checking the TRANSLUCENT flag.

	// I have to do it this way because I'm really wanting to treat alpha
	// modulation as a dynamic state, even though it's being used to compute
	// shadow state. I still want to use it to compute shadow state though
	// because it's somewhat complicated code that I'd rather precache.

	StateSnapshot_t snapshot = pRenderState->m_pSnapshots[0].m_Snapshot[0];

	// Automatically compute if the snapshot is transparent or not
	if ( g_pShaderAPI->IsTranslucent( snapshot ) )
	{
		pRenderState->m_Flags |= SHADER_OPACITY_TRANSLUCENT;
	}
	else
	{
		if ( g_pShaderAPI->IsAlphaTested( snapshot ) )
		{
			pRenderState->m_Flags |= SHADER_OPACITY_ALPHATEST;
		}
		else
		{
			pRenderState->m_Flags |= SHADER_OPACITY_OPAQUE;
		}
	}

#ifdef _DEBUG
	if( pRenderState->m_Flags & SHADER_OPACITY_TRANSLUCENT )
	{
		Assert( !( pRenderState->m_Flags & SHADER_OPACITY_ALPHATEST ) );
		Assert( !( pRenderState->m_Flags & SHADER_OPACITY_OPAQUE ) );
	}
	if( pRenderState->m_Flags & SHADER_OPACITY_ALPHATEST )
	{
		Assert( !( pRenderState->m_Flags & SHADER_OPACITY_TRANSLUCENT ) );
		Assert( !( pRenderState->m_Flags & SHADER_OPACITY_OPAQUE ) );
	}
	if( pRenderState->m_Flags & SHADER_OPACITY_OPAQUE )
	{
		Assert( !( pRenderState->m_Flags & SHADER_OPACITY_ALPHATEST ) );
		Assert( !( pRenderState->m_Flags & SHADER_OPACITY_TRANSLUCENT ) );
	}
#endif
}


//-----------------------------------------------------------------------------
// Initializes state snapshots
//-----------------------------------------------------------------------------
#ifdef _DEBUG
#pragma warning (disable:4189)
#endif

int CShaderSystem::GetModulationSnapshotCount( IMaterialVar **params )
{
	int nSnapshotCount = SnapshotTypeCount();
	return nSnapshotCount;
}

#define SET_FLAGS2B( nFlag, bState ) 			\
    if ( bState )								\
	    SET_FLAGS2( nFlag );					\
    else										\
        CLEAR_FLAGS2( nFlag );


void CShaderSystem::InitStateSnapshots( IShader *pShader, IMaterialVar **params, ShaderRenderState_t* pRenderState )
{
#ifdef _DEBUG
	if ( IsFlagSet( params, MATERIAL_VAR_DEBUG ) )
	{
		// Putcher breakpoint here to catch the rendering of a material
		// marked for debugging ($debug = 1 in a .vmt file) shadow state version
		int x = 0;
	}
#endif

	// Store off the current settings
	bool bPaint = IsFlag2Set( params, MATERIAL_VAR2_USE_PAINT );
	bool bFlashlight = IsFlag2Set( params, MATERIAL_VAR2_USE_FLASHLIGHT );
	bool bEditor = IsFlag2Set( params, MATERIAL_VAR2_USE_EDITOR );

	bool bGBuffer0 = IsFlag2Set( params, MATERIAL_VAR2_USE_GBUFFER0 );
	bool bGBuffer1 = IsFlag2Set( params, MATERIAL_VAR2_USE_GBUFFER1 );

	int nSnapshotCount = GetModulationSnapshotCount( params );

	// If the current mod does not use the flashlight, skip all flashlight snapshots (saves a ton of memory)
	bool bModUsesFlashlight = ( mat_supportflashlight.GetInt() != 0 );
	bool bCanSupportGBuffer = ( MaterialSystem()->GetConfigurationFlags() & MATCONFIG_FLAGS_SUPPORT_GBUFFER ) != 0;
	bool bCanSupportPaint = true; //FIXME g_pConfig->m_bPaintInGame;
	for ( int i = 0; i < nSnapshotCount; ++i )
	{
		// skipping this snapshot?
		if ( 
			( ( i & SHADER_USING_PAINT ) && !bCanSupportPaint ) ||												// skipping paint for mods that don't use it
			( ( i & SHADER_USING_FLASHLIGHT ) && !bModUsesFlashlight ) ||										// skipping flashlight for mods that don't use it
			( ( i & SHADER_USING_FLASHLIGHT ) && ( i & ( SHADER_USING_GBUFFER0 | SHADER_USING_GBUFFER1 ) ) ) ||	// skipping flashlight with gbuffer
			( ( i & SHADER_USING_EDITOR ) && ( !CanUseEditorMaterials() ) ) ||									// skipping editor
			( ( i & ( SHADER_USING_GBUFFER0 | SHADER_USING_GBUFFER1 ) ) && !bCanSupportGBuffer )				// skipping gbuffer if not supported
			)
		{
			pRenderState->m_pSnapshots[i].m_nPassCount = 0;
			continue;
		}

		SET_FLAGS2B( MATERIAL_VAR2_USE_FLASHLIGHT, ( i & SHADER_USING_FLASHLIGHT ) );
		SET_FLAGS2B( MATERIAL_VAR2_USE_EDITOR, ( i & SHADER_USING_EDITOR ) );
		SET_FLAGS2B( MATERIAL_VAR2_USE_PAINT, ( i & SHADER_USING_PAINT ) );
		SET_FLAGS2B( MATERIAL_VAR2_USE_GBUFFER0, ( i & SHADER_USING_GBUFFER0 ) );
		SET_FLAGS2B( MATERIAL_VAR2_USE_GBUFFER1, ( i & SHADER_USING_GBUFFER1 ) );

		PrepForShaderDraw( pShader, params, pRenderState, i );

		// Now snapshot how we're going to draw
		// NOTE: We're in a half-way point here; modulation flags should be entirely computed here
		// but I want to minimize code change while fixing the alpha-modulation related 
		// problem that the modulation flag isn't bring driven by the per-instance modulation alpha state
		pRenderState->m_pSnapshots[i].m_nPassCount = 0;
		pShader->DrawElements( params, i, g_pShaderShadow, 0, VERTEX_COMPRESSION_NONE, &(pRenderState->m_pSnapshots[i].m_pContextData[0] ), &(pRenderState->m_pSnapshots[i].m_pInstanceData[0] ) );
		DoneWithShaderDraw();
	}

	// restore flags
	SET_FLAGS2B( MATERIAL_VAR2_USE_PAINT, bPaint );
	SET_FLAGS2B( MATERIAL_VAR2_USE_EDITOR, bEditor );
	SET_FLAGS2B( MATERIAL_VAR2_USE_FLASHLIGHT, bFlashlight );
	SET_FLAGS2B( MATERIAL_VAR2_USE_GBUFFER0, bGBuffer0 );
	SET_FLAGS2B( MATERIAL_VAR2_USE_GBUFFER1, bGBuffer1 );
}

#ifdef _DEBUG
#pragma warning (default:4189)
#endif

//-----------------------------------------------------------------------------
// Helper to count texture coordinates
//-----------------------------------------------------------------------------
static int NumTextureCoordinates( VertexFormat_t vertexFormat )
{
	// FIXME: this is a duplicate of the function in meshdx8.cpp
	int nTexCoordCount = 0;
	for ( int i = 0; i < VERTEX_MAX_TEXTURE_COORDINATES; ++i )
	{
		if ( TexCoordSize( i, vertexFormat ) == 0 )
			continue;
		++nTexCoordCount;
	}
	return nTexCoordCount;
}

//-----------------------------------------------------------------------------
// Displays the vertex format
//-----------------------------------------------------------------------------
static void OutputVertexFormat( VertexFormat_t format )
{
	// FIXME: this is a duplicate of the function in meshdx8.cpp
	VertexCompressionType_t compressionType = CompressionType( format );

	if( format & VERTEX_POSITION )
	{
		Warning( "VERTEX_POSITION|" );
	}
	if( format & VERTEX_NORMAL )
	{
		if ( compressionType == VERTEX_COMPRESSION_ON )
			Warning( "VERTEX_NORMAL[COMPRESSED]|" );
		else
			Warning( "VERTEX_NORMAL|" );
	}
	if( format & VERTEX_COLOR )
	{
		Warning( "VERTEX_COLOR|" );
	}
	if( format & VERTEX_SPECULAR )
	{
		Warning( "VERTEX_SPECULAR|" );
	}
	if( format & VERTEX_TANGENT_S )
	{
		Warning( "VERTEX_TANGENT_S|" );
	}
	if( format & VERTEX_TANGENT_T )
	{
		Warning( "VERTEX_TANGENT_T|" );
	}
	if( format & VERTEX_BONE_INDEX )
	{
		Warning( "VERTEX_BONE_INDEX|" );
	}
	Warning( "\nBone weights: %d\n", NumBoneWeights( format ) );
	Warning( "user data size: %d (%s)\n", UserDataSize( format ),
		( CompressionType( format ) == VERTEX_COMPRESSION_ON ? "compressed" : "uncompressed" ) );
	Warning( "num tex coords: %d\n", NumTextureCoordinates( format ) );
	// NOTE: This doesn't print texcoord sizes.
}


#ifdef _DEBUG
static bool IsVertexFormatSubsetOfVertexformat( VertexFormat_t subset, VertexFormat_t superset )
{
	subset &= ~VERTEX_FORMAT_USE_EXACT_FORMAT;
	superset &= ~VERTEX_FORMAT_USE_EXACT_FORMAT;

	// Test the flags
	if( VertexFlags( subset ) & VertexFlags( ~superset ) )
		return false;

	// Test bone weights
	if( NumBoneWeights( subset ) > NumBoneWeights( superset ) )
		return false;
	
	// Test user data size
	if( UserDataSize( subset ) > UserDataSize( superset ) )
		return false;

	// Test the texcoord dimensions
	for( int i = 0; i < VERTEX_MAX_TEXTURE_COORDINATES; i++ )
	{
		if( TexCoordSize( i, subset ) > TexCoordSize( i, superset ) )
			return false;
	}

	return true;
}
#endif


//-----------------------------------------------------------------------------
// Adds state snapshots to the render list
//-----------------------------------------------------------------------------
static void AddSnapshotsToList( RenderPassList_t *pPassList, int &nSnapshotID, StateSnapshot_t *pSnapshots )
{
	int nNumPassSnapshots = pPassList->m_nPassCount;
	for( int i = 0; i < nNumPassSnapshots; ++i )
	{
		pSnapshots[nSnapshotID] = pPassList->m_Snapshot[i];
		nSnapshotID++;
	}
}


//-----------------------------------------------------------------------------
// Computes vertex format + usage from a particular snapshot
//-----------------------------------------------------------------------------
bool CShaderSystem::ComputeVertexFormatFromSnapshot( IMaterialVar **params, ShaderRenderState_t* pRenderState )
{
	// When computing the usage, use the snapshot that has no alpha or color
	// modulation. We need the usage + format to be the same for all
	// combinations of alpha + color modulation, though, or we are asking for
	// trouble.	
	int nModulationSnapshotCount = GetModulationSnapshotCount( params );
	int numSnapshots = pRenderState->m_pSnapshots[0].m_nPassCount;
	if (nModulationSnapshotCount >= SHADER_USING_FLASHLIGHT)
	{
		numSnapshots += pRenderState->m_pSnapshots[SHADER_USING_FLASHLIGHT].m_nPassCount;
	}
	if (nModulationSnapshotCount >= SHADER_USING_PAINT)
	{
		numSnapshots += pRenderState->m_pSnapshots[SHADER_USING_PAINT].m_nPassCount;
	}
	if ( CanUseEditorMaterials() )
	{
		numSnapshots += pRenderState->m_pSnapshots[SHADER_USING_EDITOR].m_nPassCount;
	}

	StateSnapshot_t* pSnapshots = (StateSnapshot_t*)stackalloc( 
		numSnapshots * sizeof(StateSnapshot_t) ); 

	int snapshotID = 0;
	AddSnapshotsToList( &pRenderState->m_pSnapshots[0], snapshotID, pSnapshots );
	if (nModulationSnapshotCount >= SHADER_USING_FLASHLIGHT)
	{
		AddSnapshotsToList( &pRenderState->m_pSnapshots[SHADER_USING_FLASHLIGHT], snapshotID, pSnapshots );
	}
	if (nModulationSnapshotCount >= SHADER_USING_PAINT)
	{
		AddSnapshotsToList( &pRenderState->m_pSnapshots[SHADER_USING_PAINT], snapshotID, pSnapshots );
	}
	if ( CanUseEditorMaterials() )
	{
		AddSnapshotsToList( &pRenderState->m_pSnapshots[SHADER_USING_EDITOR], snapshotID, pSnapshots );
	}

	Assert( snapshotID == numSnapshots );

	pRenderState->m_VertexUsage = g_pShaderAPI->ComputeVertexUsage( numSnapshots, pSnapshots );

#ifdef _DEBUG
	// Make sure all modulation combinations match vertex usage
	for ( int mod = 1; mod < nModulationSnapshotCount; ++mod )
	{
		int numSnapshotsTest = pRenderState->m_pSnapshots[mod].m_nPassCount;
		StateSnapshot_t* pSnapshotsTest = (StateSnapshot_t*)stackalloc( 
			numSnapshotsTest * sizeof(StateSnapshot_t) );

		for (int i = 0; i < numSnapshotsTest; ++i)
		{
			pSnapshotsTest[i] = pRenderState->m_pSnapshots[mod].m_Snapshot[i];
		}

		VertexFormat_t usageTest = g_pShaderAPI->ComputeVertexUsage( numSnapshotsTest, pSnapshotsTest );
		Assert( IsVertexFormatSubsetOfVertexformat( usageTest, pRenderState->m_VertexUsage ) );
	}
#endif

	if ( IsPC() )
	{
		pRenderState->m_VertexFormat = g_pShaderAPI->ComputeVertexFormat( numSnapshots, pSnapshots );
	}
	else
	{
		pRenderState->m_VertexFormat = pRenderState->m_VertexUsage;
	}

	return true;
}


//-----------------------------------------------------------------------------
// go through each param and make sure it is the right type, load textures, 
// compute state snapshots and vertex types, etc.
//-----------------------------------------------------------------------------
bool CShaderSystem::InitRenderState( IShader *pShader, int numParams, IMaterialVar **params, ShaderRenderState_t* pRenderState, char const* pMaterialName )
{
	Assert( !m_pRenderState );

	// Initialize render state flags
	InitRenderStateFlags( pRenderState, numParams, params );

	// Compute state snapshots for each combination of alpha + color
	InitStateSnapshots( pShader, params, pRenderState );

	// Compute other infomation for the render state based on snapshots
	if (pRenderState->m_pSnapshots[0].m_nPassCount == 0)
	{
		Warning( "Material \"%s\":\n   No render states in shader \"%s\"\n", pMaterialName, pShader->GetName() );
		return false;
	}

	// Set a couple additional flags based on the render state
	ComputeRenderStateFlagsFromSnapshot( pRenderState );

	// Compute the vertex format + usage from the snapshot
	if ( !ComputeVertexFormatFromSnapshot( params, pRenderState ) )
	{
		// warn.. return a null render state...
		Warning("Material \"%s\":\n   Shader \"%s\" can't be used with models!\n", pMaterialName, pShader->GetName() );
		CleanupRenderState( pRenderState );
		return false;
	}
	return true;
}

// When you're done with the shader, be sure to call this to clean up
void CShaderSystem::CleanupRenderState( ShaderRenderState_t* pRenderState )
{
	if (pRenderState)
	{
		int nSnapshotCount = SnapshotTypeCount();
		// kill context data
		// Indicate no passes for any of the snapshot lists
		RenderPassList_t *pTemp = pRenderState->m_pSnapshots;
		for(int i = 0; i < nSnapshotCount; i++ )
		{
			for(int j = 0 ; j < pRenderState->m_pSnapshots[i].m_nPassCount; j++ )
			{
				if ( pTemp[i].m_pContextData[j] )
				{
					delete pTemp[i].m_pContextData[j];
					pTemp[i].m_pContextData[j] = NULL;
				}
				if ( pTemp[i].m_pInstanceData[j] )
				{
					delete pTemp[i].m_pInstanceData[j];
					pTemp[i].m_pInstanceData[j] = NULL;
				}
			}
			pRenderState->m_pSnapshots[i].m_nPassCount = 0;
		}
	}
}


//-----------------------------------------------------------------------------
// Does the rendering!
//-----------------------------------------------------------------------------
void CShaderSystem::DrawElements( IShader *pShader, IMaterialVar **params, 
								  ShaderRenderState_t* pRenderState,
								  VertexCompressionType_t vertexCompression, 
								  uint32 nMaterialVarChangeTimeStamp,
								  uint32 nModulationFlags,
								  bool bRenderingPreTessPatchMesh )
{
	VPROF("CShaderSystem::DrawElements");

	g_pShaderAPI->InvalidateDelayedShaderConstants();

	// Compute modulation...
	int mod = pShader->ComputeModulationFlags( params, ShaderApiParam( g_pShaderAPI ) );
	mod |= nModulationFlags;

	// No snapshots? do nothing.
	if ( pRenderState->m_pSnapshots[mod].m_nPassCount == 0 )
		return;

	int materialVarFlags = params[FLAGS]->GetIntValue();

	// FIXME: need one conditional that we calculate once a frame for debug or not with everything debug under that.
	if ( !IsOSXOpenGL() &&
		 ( ( g_config.bMeasureFillRate || g_config.bVisualizeFillRate ) &&
		 ( ( materialVarFlags & MATERIAL_VAR_USE_IN_FILLRATE_MODE ) == 0 ) ) )
	{
		DrawMeasureFillRate( pRenderState, mod, vertexCompression );
	}
	else if( ( g_config.bShowNormalMap || g_config.nShowMipLevels == 2 ) && 
		( IsFlag2Set( params, MATERIAL_VAR2_LIGHTING_BUMPED_LIGHTMAP ) ||
		  IsFlag2Set( params, MATERIAL_VAR2_DIFFUSE_BUMPMAPPED_MODEL ) ) )
	{
		DrawNormalMap( pShader, params, vertexCompression );
	}
#if defined(DEBUG_DEPTH)
	else if ( mat_debugdepth.GetInt() && ((materialVarFlags & MATERIAL_VAR_NO_DEBUG_OVERRIDE) == 0) )
	{
		int nIndex = 0;
		if ( IsFlagSet( params, MATERIAL_VAR_DECAL ) )
		{
			nIndex |= 0x1;
		}
		IMaterialInternal *pDebugMaterial = m_pDebugMaterials[ MATERIAL_DEBUG_DEPTH + nIndex ];
		if ( !g_pShaderAPI->IsDepthWriteEnabled( pRenderState->m_Snapshots[mod].m_Snapshot[0] )	)
		{
			pDebugMaterial = m_pDebugMaterials[MATERIAL_DEBUG_WIREFRAME];
		}

		DrawUsingMaterial( pDebugMaterial, vertexCompression );
	}
#endif
	else if( bRenderingPreTessPatchMesh && !IsFlag2Set( params, MATERIAL_VAR2_SUPPORTS_TESSELLATION ) )
	{
		Warning( "Warning error: CShaderSystem::DrawElements: Mesh supports tessellation, but material does not.\n" );
		DrawUsingMaterial( g_pErrorMaterial, vertexCompression );
	}
	else
	{
		g_pShaderAPI->SetDefaultState();

		// If we're rendering flat, turn on flat mode...
		if (materialVarFlags & MATERIAL_VAR_FLAT)
		{
			g_pShaderAPI->ShadeMode( SHADER_FLAT );
		}

		PrepForShaderDraw( pShader, params, pRenderState, mod );
		g_pShaderAPI->BeginPass( CurrentStateSnapshot() );
		
		CBasePerMaterialContextData ** pContextDataPtr = 
			&( m_pRenderState->m_pSnapshots[m_nModulation].m_pContextData[0] );

		if ( *pContextDataPtr && ( (*pContextDataPtr)->m_nVarChangeID != nMaterialVarChangeTimeStamp ) )
		{
			(*pContextDataPtr)->m_bMaterialVarsChanged = true;
			(*pContextDataPtr)->m_nVarChangeID = nMaterialVarChangeTimeStamp;
		}

		pShader->DrawElements( 
			params, mod, 0, ShaderApiParam( g_pShaderAPI ), vertexCompression, pContextDataPtr,
			&( m_pRenderState->m_pSnapshots[m_nModulation].m_pInstanceData[0] ) );
		DoneWithShaderDraw();
	}

	MaterialSystem()->ForceDepthFuncEquals( false );
}


//-----------------------------------------------------------------------------
// Are we using graphics?
//-----------------------------------------------------------------------------
bool CShaderSystem::IsUsingGraphics() const
{
	// YWB Hack if running with -noshaderapi/-makereslists this forces materials to "precache" which means they will resolve their .vtf files for
	//  things like normal/height/dudv maps...
	if ( m_bForceUsingGraphicsReturnTrue )
		return true;

	return g_pShaderDevice->IsUsingGraphics();
}


//-----------------------------------------------------------------------------
// Are we using the editor materials?
//-----------------------------------------------------------------------------
bool CShaderSystem::CanUseEditorMaterials() const
{
	return MaterialSystem()->GetConfigurationFlags() & MATCONFIG_FLAGS_SUPPORT_EDITOR;
}


//-----------------------------------------------------------------------------
// Takes a snapshot
//-----------------------------------------------------------------------------
void CShaderSystem::TakeSnapshot( )
{
	Assert( m_pRenderState );
	Assert( m_nModulation < SnapshotTypeCount() );

	if ( g_pHardwareConfig->GetDXSupportLevel() >= 92 )
	{
		//enable linear->gamma srgb conversion lookup texture
		g_pShaderShadow->EnableTexture( SHADER_SAMPLER15, true );
		g_pShaderShadow->EnableSRGBRead( SHADER_SAMPLER15, true );
	}
	
	RenderPassList_t& snapshotList = m_pRenderState->m_pSnapshots[m_nModulation];

	// Take a snapshot...
	snapshotList.m_Snapshot[snapshotList.m_nPassCount] = g_pShaderAPI->TakeSnapshot();
	++snapshotList.m_nPassCount;
	Assert( snapshotList.m_nPassCount <= MAX_RENDER_PASSES );
}


//-----------------------------------------------------------------------------
// Draws a snapshot
//-----------------------------------------------------------------------------
void CShaderSystem::DrawSnapshot( const unsigned char *pInstanceCommandBuffer, bool bMakeActualDrawCall )
{
	Assert( m_pRenderState );
	RenderPassList_t& snapshotList = m_pRenderState->m_pSnapshots[m_nModulation];

	int nPassCount = snapshotList.m_nPassCount;
	Assert( m_nRenderPass < nPassCount );

	if ( bMakeActualDrawCall )
	{
		g_pShaderAPI->RenderPass( pInstanceCommandBuffer, m_nRenderPass, nPassCount );
	}

	g_pShaderAPI->InvalidateDelayedShaderConstants();
	if (++m_nRenderPass < nPassCount)
	{
		g_pShaderAPI->BeginPass( CurrentStateSnapshot() );
	}
}



//-----------------------------------------------------------------------------
//
// Debugging material methods below
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Draws a using a particular material..
//-----------------------------------------------------------------------------
void CShaderSystem::DrawUsingMaterial( IMaterialInternal *pMaterial, VertexCompressionType_t vertexCompression )
{
	ShaderRenderState_t *pRenderState = pMaterial->GetRenderState();
	g_pShaderAPI->SetDefaultState( );

	IShader *pShader = pMaterial->GetShader();
	int nMod = pShader->ComputeModulationFlags( pMaterial->GetShaderParams(), ShaderApiParam( g_pShaderAPI ) );
	PrepForShaderDraw( pShader, pMaterial->GetShaderParams(), pRenderState, nMod );
	g_pShaderAPI->BeginPass( pRenderState->m_pSnapshots[nMod].m_Snapshot[0] );
	pShader->DrawElements( pMaterial->GetShaderParams(), nMod, 0, ShaderApiParam( g_pShaderAPI ), vertexCompression, 
						   &( pRenderState->m_pSnapshots[nMod].m_pContextData[0] ),
						   &( pRenderState->m_pSnapshots[nMod].m_pInstanceData[0] ));
	DoneWithShaderDraw( );
}


//-----------------------------------------------------------------------------
// Copies material vars
//-----------------------------------------------------------------------------
void CShaderSystem::CopyMaterialVarToDebugShader( IMaterialInternal *pDebugMaterial, IShader *pShader, IMaterialVar **ppParams, const char *pSrcVarName, const char *pDstVarName )
{
	bool bFound;
	IMaterialVar *pMaterialVar = pDebugMaterial->FindVar( pDstVarName ? pDstVarName : pSrcVarName, &bFound );
	Assert( bFound );

	for( int i = pShader->GetParamCount(); --i >= 0; )
	{
		if( !Q_stricmp( ppParams[i]->GetName( ), pSrcVarName ) )
		{
			pMaterialVar->CopyFrom( ppParams[i] );
			return;
		}
	}

	pMaterialVar->SetUndefined();
}


//-----------------------------------------------------------------------------
// Draws the puppy in fill rate mode...
//-----------------------------------------------------------------------------
void CShaderSystem::DrawMeasureFillRate( ShaderRenderState_t* pRenderState, int mod, VertexCompressionType_t vertexCompression )
{
	int nPassCount = pRenderState->m_pSnapshots[mod].m_nPassCount;

	IMaterialInternal *pMaterial = m_pDebugMaterials[ MATERIAL_FILL_RATE ];

	bool bFound;
	IMaterialVar *pMaterialVar = pMaterial->FindVar( "$passcount", &bFound );
	pMaterialVar->SetIntValue( nPassCount );
	DrawUsingMaterial( pMaterial, vertexCompression );
}


//-----------------------------------------------------------------------------
// Draws normalmaps
//-----------------------------------------------------------------------------
void CShaderSystem::DrawNormalMap( IShader *pShader, IMaterialVar **ppParams, VertexCompressionType_t vertexCompression )
{
	IMaterialInternal *pDebugMaterial = m_pDebugMaterials[MATERIAL_DEBUG_NORMALMAP];  
	
	if( !g_config.m_bFastNoBump )
	{
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$bumpmap" );
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$bumpframe" );
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$bumptransform" );
	}
	else
	{
		bool bFound;
		IMaterialVar *pMaterialVar = pDebugMaterial->FindVar( "$bumpmap", &bFound );
		Assert( bFound );

		pMaterialVar->SetUndefined();
	}

	DrawUsingMaterial( pDebugMaterial, vertexCompression );
}

//-----------------------------------------------------------------------------
// Draws envmapmask
//-----------------------------------------------------------------------------
bool CShaderSystem::DrawEnvmapMask( IShader *pShader, IMaterialVar **ppParams, 
								   ShaderRenderState_t* pRenderState, VertexCompressionType_t vertexCompression )
{
	// FIXME!  Make this work with fixed function.
	IMaterialInternal *pDebugMaterial = m_pDebugMaterials[ MATERIAL_DEBUG_ENVMAPMASK ];

	bool bFound;
	IMaterialVar *pMaterialVar = pDebugMaterial->FindVar( "$showalpha", &bFound );
	Assert( bFound );

	if( IsFlagSet( ppParams, MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK ) )
	{
		// $bumpmap
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$bumpmap", "$basetexture" );
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$bumpframe", "$frame" );
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$bumptransform", "$basetexturetransform" );
		pMaterialVar->SetIntValue( 1 );
	}
	else if( IsFlagSet( ppParams, MATERIAL_VAR_BASEALPHAENVMAPMASK ) )
	{
		// $basealphaenvmapmask
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$basetexture" );
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$frame" );
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$basetexturetransform" );
		pMaterialVar->SetIntValue( 1 );
	}
	else
	{
		// $envmapmask
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$envmapmask", "$basetexture" );
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$envmapmaskframe", "$frame" );
		CopyMaterialVarToDebugShader( pDebugMaterial, pShader, ppParams, "$envmapmasktransform", "$basetexturetransform" );
		pMaterialVar->SetIntValue( 0 );
	}

	if( pDebugMaterial->FindVar( "$basetexture", NULL )->IsTexture() )
	{
		DrawUsingMaterial( pDebugMaterial, vertexCompression );
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------
//
// Methods of IShaderSystem lie below
//
//-----------------------------------------------------------------------------


ShaderAPITextureHandle_t CShaderSystem::GetShaderAPITextureBindHandle( ITexture *pTexture, int nFrame, int nTextureChannel )
{
	Assert( !IsTextureInternalEnvCubemap( static_cast<ITextureInternal*>(pTexture) ) );

	// Bind away baby
	if( pTexture )
	{
		return static_cast<ITextureInternal*>(pTexture)->GetTextureHandle( nFrame, nTextureChannel );
	}
	else
		return INVALID_SHADERAPI_TEXTURE_HANDLE;
}

//-----------------------------------------------------------------------------
// Binds a texture
//-----------------------------------------------------------------------------
void CShaderSystem::BindTexture( Sampler_t sampler1, TextureBindFlags_t nBindFlags, ITexture *pTexture, int nFrame /* = 0 */ )
{
	// The call to IMaterialVar::GetTextureValue should have converted this to a real thing
	Assert( !IsTextureInternalEnvCubemap( static_cast<ITextureInternal*>(pTexture) ) );

	// Bind away baby
	if( pTexture )
	{
		static_cast<ITextureInternal*>(pTexture)->Bind( sampler1, nBindFlags, nFrame );
	}
}


void CShaderSystem::BindTexture( Sampler_t sampler1, Sampler_t sampler2, TextureBindFlags_t nBindFlags, ITexture *pTexture, int nFrame /* = 0 */ )
{
	// The call to IMaterialVar::GetTextureValue should have converted this to a real thing
	Assert( !IsTextureInternalEnvCubemap( static_cast<ITextureInternal*>(pTexture) ) );

	// Bind away baby
	if( pTexture )
	{
		if ( sampler2 == -1 )
		{
			static_cast<ITextureInternal*>(pTexture)->Bind( sampler1, nBindFlags, nFrame );
		}
		else
		{
			static_cast<ITextureInternal*>(pTexture)->Bind( sampler1, nBindFlags, nFrame, sampler2 );
		}
	}
}


void CShaderSystem::BindVertexTexture( VertexTextureSampler_t vtSampler, ITexture *pTexture, int nFrame /* = 0 */ )
{
	if( pTexture )
	{
		static_cast<ITextureInternal*>(pTexture)->BindVertexTexture( vtSampler, nFrame );
	}
}



//-----------------------------------------------------------------------------
//
// Methods of IShaderInit lie below
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Loads a texture
//-----------------------------------------------------------------------------
void CShaderSystem::LoadTexture( IMaterialVar *pTextureVar, const char *pTextureGroupName, int nAdditionalCreationFlags /* = 0 */ )
{
	if (pTextureVar->GetType() != MATERIAL_VAR_TYPE_STRING)
	{
		// This here will cause 'UNDEFINED' material vars
		if (pTextureVar->GetType() != MATERIAL_VAR_TYPE_TEXTURE)
		{
			pTextureVar->SetTextureValue( TextureManager()->ErrorTexture() );
		}
		return;
	}

	// In this case, we have to convert the string into a texture value
	const char *pName = pTextureVar->GetStringValue();
	
	// Fix cases where people stupidly put a slash at the front of the vtf filename in a vmt. Causes trouble elsewhere.
	if ( pName[0] == CORRECT_PATH_SEPARATOR || pName[1] == CORRECT_PATH_SEPARATOR ) 
		++pName;

	ITextureInternal *pTexture;

	// Force local cubemaps when using the editor
	if ( CanUseEditorMaterials() && ( stricmp( pName, "env_cubemap" ) == 0 ) )
	{
		pTexture = (ITextureInternal*)-1;
	}
	else
	{
		pTexture = TextureManager()->FindOrLoadTexture( pName, pTextureGroupName, nAdditionalCreationFlags );
	}

	if( !pTexture )
	{
		if( !g_pShaderDevice->IsUsingGraphics() && ( stricmp( pName, "env_cubemap" ) != 0 ) )
		{
			Warning( "Shader_t::LoadTexture: texture \"%s.vtf\" doesn't exist\n", pName );
		}
		pTexture = TextureManager()->ErrorTexture();
	}

	pTextureVar->SetTextureValue( pTexture );
}


//-----------------------------------------------------------------------------
// Loads a bumpmap
//-----------------------------------------------------------------------------
void CShaderSystem::LoadBumpMap( IMaterialVar *pTextureVar, const char *pTextureGroupName, int nAdditionalCreationFlags )
{
	Assert( pTextureVar );

	if (pTextureVar->GetType() != MATERIAL_VAR_TYPE_STRING)
	{
		// This here will cause 'UNDEFINED' material vars
		if (pTextureVar->GetType() != MATERIAL_VAR_TYPE_TEXTURE)
		{
			pTextureVar->SetTextureValue( TextureManager()->ErrorTexture() );
		}
		return;
	}

	// Convert a string to the actual texture
	ITextureInternal *pTexture;
	pTexture = TextureManager()->FindOrLoadTexture( pTextureVar->GetStringValue(), pTextureGroupName, nAdditionalCreationFlags );

	// FIXME: Make a bumpmap error texture
	if (!pTexture)
	{
		pTexture = TextureManager()->ErrorTexture();
	}

	pTextureVar->SetTextureValue( pTexture );
}

//-----------------------------------------------------------------------------
// Loads a cubemap
//-----------------------------------------------------------------------------
void CShaderSystem::LoadCubeMap( IMaterialVar **ppParams, IMaterialVar *pTextureVar, int nAdditionalCreationFlags /* = 0 */ )
{
	if ( pTextureVar->GetType() != MATERIAL_VAR_TYPE_STRING )
	{
		// This here will cause 'UNDEFINED' material vars
		if (pTextureVar->GetType() != MATERIAL_VAR_TYPE_TEXTURE)
		{
			pTextureVar->SetTextureValue( TextureManager()->ErrorTexture() );
		}
		return;
	}

	if ( stricmp( pTextureVar->GetStringValue(), "env_cubemap" ) == 0 )
	{
		// garymcthack 
		// don't have to load anything here. . just set the texture value to something
		// special that says to use the cubemap entity.
		pTextureVar->SetTextureValue( ( ITexture * )-1 );
		SetFlags2( ppParams, MATERIAL_VAR2_USES_ENV_CUBEMAP );
	}
	else
	{
		ITextureInternal *pTexture;
		char textureName[MAX_PATH];
		Q_strncpy( textureName, pTextureVar->GetStringValue(), MAX_PATH );
		if ( HardwareConfig()->GetHDRType() != HDR_TYPE_NONE )
		{
			// Overload the texture name to ".hdr.vtf" (instead of .vtf) if we are running with 
			// HDR enabled.
			Q_strncat( textureName, ".hdr", MAX_PATH, COPY_ALL_CHARACTERS );
		}
		pTexture = TextureManager()->FindOrLoadTexture( textureName, TEXTURE_GROUP_CUBE_MAP, nAdditionalCreationFlags );

		// FIXME: Make a cubemap error texture
		if ( !pTexture )
		{
			pTexture = TextureManager()->ErrorTexture();
		}

		pTextureVar->SetTextureValue( pTexture );
	}
}

void CShaderSystem::AddShaderComboInformation( const ShaderComboSemantics_t *pSemantics )
{
	g_pShaderAPI->AddShaderComboInformation( pSemantics );
}

#ifdef _PS3
#include "shadersystem_ps3nonvirt.inl"
#endif

