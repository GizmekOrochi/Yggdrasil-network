#pragma once

/**
 * @file PutFileService.hpp
 * @brief Orchestrateur de depot durable et chiffre.
 */

#include "ygg/storage/IPlacementPlanner.hpp"
#include "ygg/storage/IImportJournal.hpp"
#include "ygg/storage/IRemoteShardClient.hpp"
#include "ygg/storage/IManifestStore.hpp"
#include "ygg/storage/IPrivateCatalog.hpp"
#include "ygg/storage/IEncryptedCatalogBackup.hpp"
#include "ygg/storage/IErasureCodec.hpp"
#include "ygg/crypto/IContentCipher.hpp"

namespace ygg::storage {
/**
 * @brief Transaction de publication d une nouvelle version.
 * @details Les etapes sont journalisees; apres un crash, reprendre sans annoncer un etat plus
 * durable que la realite.
 * @par Responsabilite du fichier Orchestration de depot de fichiers chiffrés.
 * @par Regles importantes Ne jamais retourner un succes PROTECTED tant que les receipts, le
 * manifeste, le catalogue et son backup n ont pas été persistes. La signature, la DEK et l identite
 * client doivent etre integrees avant implementation.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Owner-side streaming import coordinator. Per segment: AEAD -> XOR shares -> durable remote
 * receipts. Then sign and replicate manifest, commit encrypted private index, replicate catalog
 * backup and mark protected only when full recovery is possible.
 */
class PutFileService final {
public:
    /**
     * @brief Injecte placement, journal, chiffrement, codec, RPC shards, manifestes, catalogue et
     * backup.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param placement Selecteur de destinations independantes injecte.
     * @param journal Journal de progression et de durabilite injecte.
     * @param cipher Argument fourni par l appelant; validite et contraintes definies par ce
     * contrat.
     * @param coding Codec XOR chiffre injecte, ne doit pas voir les secrets.
     * @param remote Client RPC des shards distants injecte.
     * @param manifests Persistance distante des manifestes immutable injectee.
     * @param catalog Catalogue prive local injecte et gere hors du service.
     * @param backup Service de sauvegarde distante du catalogue deja chiffre.
     */
    PutFileService(IPlacementPlanner& placement, IImportJournal& journal, crypto::IContentCipher& cipher, IErasureCodec& coding, IRemoteShardClient& remote, IManifestStore& manifests, IPrivateCatalog& catalog, IEncryptedCatalogBackup& backup);
    /**
     * @brief Orchestre une transaction : segmenter, chiffrer, coder, stocker 3 shards, publier
     * manifeste, commit catalogue, backup puis protected.
     * @param source Chemin local du fichier clair a importer.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @attention La signature actuelle ne contient pas explicitement de signer, generateur de DEK
     * ni KeyEnvelope : cette lacune de composition devra etre resolue avant tout code fonctionnel.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<FileRef> put(Path source);
private:
    /**
     * @brief Chiffre localement le segment, produit les trois shards, choisit les destinations et
     * exige 3 reçus durables.
     * @param file Reference stable de la version de fichier concernée.
     * @param index Indice du segment dans le fichier (ordre croissant).
     * @param clear_segment Segment en clair possede, jamais transmis aux pairs de stockage.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @post Trois shards sur trois domaines physiques distincts doivent etre confirmes durablement.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<void> process_segment(FileRef file, std::uint64_t index, Bytes clear_segment);
    /**
     * @brief Planificateur de destinations externes.
     */
    IPlacementPlanner& placement_;
    /**
     * @brief Journal de progression durable injecte.
     */
    IImportJournal& journal_;
    /**
     * @brief Service AEAD injecte; seul le client peut l utiliser avec une DEK.
     */
    crypto::IContentCipher& cipher_;
    /**
     * @brief Codec d effacement injecte, sans connaissance des cles.
     */
    IErasureCodec& coding_;
    /**
     * @brief Acces RPC aux shards stockes par des pairs.
     */
    IRemoteShardClient& remote_;
    /**
     * @brief Store de manifestes injecte, non possede par le service.
     */
    IManifestStore& manifests_;
    /**
     * @brief Catalogue prive injecte, non possede par le service.
     */
    IPrivateCatalog& catalog_;
    /**
     * @brief Sauvegarde chiffrée du catalogue injectee.
     */
    IEncryptedCatalogBackup& backup_;
};
} // namespace ygg::storage
