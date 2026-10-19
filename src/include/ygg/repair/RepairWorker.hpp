#pragma once

/**
 * @file RepairWorker.hpp
 * @brief Reparateur de shards chiffres sans acces aux DEK.
 */

#include "ygg/repair/IRepairJournal.hpp"
#include "ygg/storage/IRemoteShardClient.hpp"
#include "ygg/storage/IErasureCodec.hpp"
#include "ygg/storage/IPlacementRegistry.hpp"

namespace ygg::repair {

/**
 * @brief Travailleur regenerant un shard exclusivement chiffre.
 * @details Reconstruit, compare au digest du manifeste, enregistre a distance puis publie une
 * annonce de localisation.
 * @par Responsabilite du fichier Execution de la regeneration de shards chiffres.
 * @par Regles importantes Ne manipule jamais de DEK ni de plaintext; verifier le digest du shard
 * reconstruit avant sa publication.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Reconstruct only encrypted XOR shares without any decryption key. Checks signed manifest
 * digests, obtains a durable target receipt and only then publishes a signed placement
 * advertisement.
 */
class RepairWorker final {
public:
    /**
     * @brief Injecte uniquement les ports de shards chiffres, codec, registre et journal.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param shards Fragments a decoder ou client RPC injecte selon la signature.
     * @param coding Codec XOR chiffre injecte, ne doit pas voir les secrets.
     * @param registry Registre des placements mutable injecte.
     * @param journal Journal de progression et de durabilite injecte.
     */
    RepairWorker(storage::IRemoteShardClient& shards, storage::IErasureCodec& coding, storage::IPlacementRegistry& registry, IRepairJournal& journal);
    /**
     * @brief Lit deux shards, verifie leurs digests, reconstruit le troisieme, exige receipt
     * durable et annonce placement.
     * @param plan Instructions de reconstruction deja validees.
     * @param manifest Manifeste signe de la version et de ses segments.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @post L annonce de localisation est publiee uniquement apres receipt durable du shard
     * reconstruit.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<RepairOutcome> execute(RepairPlan plan, storage::SignedManifest manifest);
private:
    /**
     * @brief Client RPC de shards injecte, non possede par le service.
     */
    storage::IRemoteShardClient& shards_;
    /**
     * @brief Codec d effacement injecte, sans connaissance des cles.
     */
    storage::IErasureCodec& coding_;
    /**
     * @brief Registre d emplacements injecte, sans propriete transfert.
     */
    storage::IPlacementRegistry& registry_;
    /**
     * @brief Journal de progression durable injecte.
     */
    IRepairJournal& journal_;
};
} // namespace ygg::repair
