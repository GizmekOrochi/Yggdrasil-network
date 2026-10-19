#pragma once
/**
 * @brief Panorama des couches publiques : core -> protocol -> identity/net -> crypto -> storage ->
 * repair -> daemon/cli.
 * @details Rassemble les declarations de core, protocole, identite, reseau, chiffrement, stockage,
 * reparations et commandes locales.
 * @par Frontieres net ne connait aucun fichier; storage ne depend pas de Boost.Asio concret; daemon
 * compose toutes les dependances.
 * @par Implementation Les consommateurs peuvent inclure un en-tete specialise pour reduire le
 * couplage. Ce fichier ne doit introduire ni etat global ni implementation.
 */


/**
 * @file Yggdrasil.hpp
 * @brief Public umbrella header for all Yggdrasil C++20 contracts.
 *
 * Individual <ygg/...> headers remain available for modular consumers.
 * Headers are interface declarations only: implementations are intentionally
 * absent from the source skeleton.
 */

#include "ygg/core/Ids.hpp"
#include "ygg/core/Result.hpp"
#include "ygg/core/Types.hpp"
#include "ygg/core/Config.hpp"
#include "ygg/core/Clock.hpp"
#include "ygg/core/Observability.hpp"
#include "ygg/protocol/Frame.hpp"
#include "ygg/protocol/FrameCodec.hpp"
#include "ygg/protocol/Messages.hpp"
#include "ygg/protocol/MessageRouter.hpp"
#include "ygg/protocol/RequestTracker.hpp"
#include "ygg/protocol/ChunkTransfer.hpp"
#include "ygg/identity/NodeIdentity.hpp"
#include "ygg/identity/IdentityStore.hpp"
#include "ygg/identity/PeerTrustStore.hpp"
#include "ygg/identity/CertificateVerifier.hpp"
#include "ygg/net/IChannel.hpp"
#include "ygg/net/ITransport.hpp"
#include "ygg/net/TcpTlsTransport.hpp"
#include "ygg/net/SecureChannel.hpp"
#include "ygg/net/RetryPolicy.hpp"
#include "ygg/net/PeerSession.hpp"
#include "ygg/net/PeerManager.hpp"
#include "ygg/net/IPeerDiscovery.hpp"
#include "ygg/net/MdnsDiscovery.hpp"
#include "ygg/net/ManualDiscovery.hpp"
#include "ygg/crypto/CryptoTypes.hpp"
#include "ygg/crypto/IContentCipher.hpp"
#include "ygg/crypto/IKeyEnvelope.hpp"
#include "ygg/crypto/ISignatureService.hpp"
#include "ygg/crypto/SodiumXChaChaCipher.hpp"
#include "ygg/crypto/SodiumSealedBoxEnvelope.hpp"
#include "ygg/crypto/SodiumEd25519Signer.hpp"
#include "ygg/storage/Domain.hpp"
#include "ygg/storage/IErasureCodec.hpp"
#include "ygg/storage/Xor2Plus1Codec.hpp"
#include "ygg/storage/IShardStore.hpp"
#include "ygg/storage/DiskShardStore.hpp"
#include "ygg/storage/IRemoteShardClient.hpp"
#include "ygg/storage/RpcShardClient.hpp"
#include "ygg/storage/IManifestStore.hpp"
#include "ygg/storage/ReplicatedManifestStore.hpp"
#include "ygg/storage/IPrivateCatalog.hpp"
#include "ygg/storage/SQLitePrivateCatalog.hpp"
#include "ygg/storage/IEncryptedCatalogBackup.hpp"
#include "ygg/storage/ReplicatedCatalogBackup.hpp"
#include "ygg/storage/IImportJournal.hpp"
#include "ygg/storage/SQLiteImportJournal.hpp"
#include "ygg/storage/IPlacementPlanner.hpp"
#include "ygg/storage/DistinctNodePlanner.hpp"
#include "ygg/storage/IPlacementRegistry.hpp"
#include "ygg/storage/PutFileService.hpp"
#include "ygg/storage/GetFileService.hpp"
#include "ygg/repair/RepairTypes.hpp"
#include "ygg/repair/IRepairJournal.hpp"
#include "ygg/repair/AvailabilityScanner.hpp"
#include "ygg/repair/RepairPlanner.hpp"
#include "ygg/repair/RepairWorker.hpp"
#include "ygg/repair/RepairCoordinator.hpp"
#include "ygg/daemon/LocalApi.hpp"
#include "ygg/daemon/LocalApiServer.hpp"
#include "ygg/daemon/YggDaemon.hpp"
#include "ygg/cli/LocalApiClient.hpp"
