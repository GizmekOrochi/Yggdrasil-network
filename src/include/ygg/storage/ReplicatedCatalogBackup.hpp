#pragma once

/**
 * @file ReplicatedCatalogBackup.hpp
 * @brief Replication des snapshots privés chiffrés.
 */

#include "ygg/storage/IEncryptedCatalogBackup.hpp"
#include "ygg/protocol/MessageRouter.hpp"

namespace ygg::storage {
    
/**
 * @brief Composant ReplicatedCatalogBackup du module storage.
 * @details Diffuse un snapshot déjà chiffre vers trois domaines de panne et retrouve une copie
 * valide.
 * @par Responsabilite du fichier Adaptateur des backups prives répliqués.
 * @par Regles importantes Verifier la durabilite et l integrite des copies, et ne jamais decrypter
 * le catalogue sur les serveurs de stockage.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Replicate encrypted snapshots on independent hosts, with receipts.
 */
class ReplicatedCatalogBackup final : public IEncryptedCatalogBackup {
public:
    /**
     * @brief Injecte le routeur pour les appels vers les detenteurs de sauvegardes chiffrees.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param router Routeur RPC generique injecte.
     */
    explicit ReplicatedCatalogBackup(protocol::MessageRouter& router);
    /**
     * @brief Ecrit des copies dans des domaines independants et retourne un SnapshotId stable.
     * @param encrypted_snapshot Sauvegarde privee deja chiffree du catalogue.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @warning La replique doit etre chiffree cote client; le serveur ne gere pas la cle de
     * restauration.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<SnapshotId> publish(Bytes encrypted_snapshot) override;
    /**
     * @brief Charge et verifie un snapshot valide sans exposer de metadonnees claires aux pairs.
     * @param id Identifiant opaque de l objet recherche ou modifie.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<Bytes> restore(SnapshotId id) override;
private:
    /**
     * @brief Routeur RPC injecte, sans dependance a un transport concret.
     */
    protocol::MessageRouter& router_;
};
} // namespace ygg::storage
