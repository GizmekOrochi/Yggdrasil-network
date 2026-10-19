#pragma once

/**
 * @file GetFileService.hpp
 * @brief Orchestrateur de lecture authentifiee sur fragments.
 */

#include "ygg/storage/IPrivateCatalog.hpp"
#include "ygg/storage/IManifestStore.hpp"
#include "ygg/storage/IRemoteShardClient.hpp"
#include "ygg/storage/IErasureCodec.hpp"
#include "ygg/crypto/IContentCipher.hpp"
#include "ygg/crypto/IKeyEnvelope.hpp"

namespace ygg::storage {
    
/**
 * @brief Transaction locale de lecture sans exposition du clair partiel.
 * @details Ecrire dans un fichier temporaire restreint et publier par renommage atomique seulement
 * apres verification de tous les segments.
 * @par Responsabilite du fichier Orchestration de la lecture et restauration cliente.
 * @par Regles importantes Ne jamais publier de fichier partiel; l API actuelle ne montre pas
 * explicitement le registre de placement ni le recuperateur de cles client : a definir avant
 * implementation.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Restore a complete verified file from any two healthy XOR shards. Never publish incomplete
 * plaintext; write restricted temporary output, verify all segment AEAD tags, then atomically
 * rename the completed file.
 */
class GetFileService final {
public:
    /**
     * @brief Injecte les dependances de restauration et de dechiffrement sous la responsabilite du
     * client.
     * @param catalog Catalogue prive local injecte et gere hors du service.
     * @param manifests Persistance distante des manifestes immutable injectee.
     * @param shards Fragments a decoder ou client RPC injecte selon la signature.
     * @param coding Codec XOR chiffre injecte, ne doit pas voir les secrets.
     * @param cipher Argument fourni par l appelant; validite et contraintes definies par ce
     * contrat.
     * @param envelope Service de deballage des DEK possede par le client.
     */
    GetFileService(IPrivateCatalog& catalog, IManifestStore& manifests,IRemoteShardClient& shards, IErasureCodec& coding, crypto::IContentCipher& cipher, crypto::IKeyEnvelope& envelope);
    /**
     * @brief Reconstruit chaque segment dans un fichier temporaire protege, verifie AEAD et renomme
     * atomiquement le fichier complet.
     * @param file Reference stable de la version de fichier concernée.
     * @param destination Adresse distante a contacter ou destination locale de sortie selon le
     * contexte.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @attention La resolution des locations et la possession de la cle privee de destinataire ne
     * sont pas exposees par les dependances actuelles : integration a specifier.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<void> get(FileRef file, Path destination);
private:
    /**
     * @brief Obtient deux shards d indices differents, verifie leurs hash et recree le ciphertext
     * exact avant dechiffrement.
     * @param segment Description signee du segment a reconstruire.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @warning Les hashes de shards et la signature du manifeste doivent etre controles en plus du
     * dechiffrement AEAD.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<Bytes> rebuild_segment(const SegmentDescriptor& segment);
    /**
     * @brief Catalogue prive injecte, non possede par le service.
     */
    IPrivateCatalog& catalog_;
    /**
     * @brief Store de manifestes injecte, non possede par le service.
     */
    IManifestStore& manifests_;
    /**
     * @brief Client RPC de shards injecte, non possede par le service.
     */
    IRemoteShardClient& shards_;
    /**
     * @brief Codec d effacement injecte, sans connaissance des cles.
     */
    IErasureCodec& coding_;
    /**
     * @brief Service AEAD injecte; seul le client peut l utiliser avec une DEK.
     */
    crypto::IContentCipher& cipher_;
    /**
     * @brief Service de deballage de DEK injecte cote client.
     */
    crypto::IKeyEnvelope& envelope_;
};
} // namespace ygg::storage
