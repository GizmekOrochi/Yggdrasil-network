#pragma once

/**
 * @file RepairTypes.hpp
 * @brief Rapports de disponibilite et plans de reconstruction.
 */

#include "ygg/storage/Domain.hpp"
#include <array>
#include <cstdint>

namespace ygg::repair {
    
/**
 * @brief Liste des etats/codes stables de SegmentState.
 * @details Les valeurs sont contractuelles; les transitions et interpretations doivent etre
 * explicitement validees.
 * @par Responsabilite du fichier DTO de sante, plans et progression de regeneration.
 * @par Regles importantes La disponibilite depend du nombre de shards distincts valides; les etats
 * ne prouvent pas automatiquement l integrite en l absence de verification.

 * @par Contrat complementaire initial
 * Availability classification, distinct from permanent data loss.
 */
enum class SegmentState {
    healthy,
    degraded,
    unavailable,
    corrupt
};
/**
 * @brief Etat de sante d un segment precis.
 * @details Chaque bool correspond a un indice de shard : deux indices valides suffisent a lire,
 * mais pas a revendiquer la redondance initiale.
 * @par Responsabilite du fichier DTO de sante, plans et progression de regeneration.
 * @par Regles importantes La disponibilite depend du nombre de shards distincts valides; les etats
 * ne prouvent pas automatiquement l integrite en l absence de verification.
 */
struct HealthReport {
    /**
     * @brief Manifeste signe correspondant au segment examine.
     */
    ManifestId manifest{};
    /**
     * @brief Position du segment concerne dans le manifeste.
     */
    std::uint64_t segment_index{};
    /**
     * @brief Booleens representant la presence de 3 shards de distincts indices.
     */
    std::array<bool,3> healthy_shards{};
    /**
     * @brief Statut de sante ou de travail a persister selon la structure.
     */
    SegmentState state{SegmentState::unavailable};
};

/**
 * @brief Plan immutable pour reconstituer UN shard manquant.
 * @details Le digest repair_key identifie un travail de maniere stable afin d eviter les
 * duplications concurrentes.
 * @par Responsabilite du fichier DTO de sante, plans et progression de regeneration.
 * @par Regles importantes La disponibilite depend du nombre de shards distincts valides; les etats
 * ne prouvent pas automatiquement l integrite en l absence de verification.

 * @par Contrat complementaire initial
 * Idempotent immutable repair instruction.
 */
struct RepairPlan {
    /**
     * @brief Manifeste signe correspondant au segment examine.
     */
    ManifestId manifest{};
    /**
     * @brief Position du segment concerne dans le manifeste.
     */
    std::uint64_t segment_index{};
    /**
     * @brief Indice (0=A,1=B,2=P) du shard a reconstruire.
     */
    std::uint8_t missing_shard_index{};
    /**
     * @brief Champ target de RepairPlan : valeur interne conforme aux invariants de ce type.
     */
    PeerId target{};
    /**
     * @brief Digest canonique permettant d eviter la regeneration double.
     */
    Digest repair_key{};
};

/**
 * @brief Resultat materiel de la regeneration.
 * @details durable ne devient vrai qu apres confirmation de depot; placement contient l annonce
 * pouvant alors etre publiee.
 * @par Responsabilite du fichier DTO de sante, plans et progression de regeneration.
 * @par Regles importantes La disponibilite depend du nombre de shards distincts valides; les etats
 * ne prouvent pas automatiquement l integrite en l absence de verification.

 * @par Contrat complementaire initial
 * Repair result is not a plaintext recovery statement.
 */
struct RepairOutcome {
    /**
     * @brief Vrai uniquement apres reception du receipt durable cible.
     */
    bool durable{};
    /**
     * @brief Champ placement de RepairOutcome : valeur interne conforme aux invariants de ce type.
     */
    storage::PlacementClaim placement{};
};

/**
 * @brief Liste des etats/codes stables de RepairState.
 * @details Les valeurs sont contractuelles; les transitions et interpretations doivent etre
 * explicitement validees.
 * @par Responsabilite du fichier DTO de sante, plans et progression de regeneration.
 * @par Regles importantes La disponibilite depend du nombre de shards distincts valides; les etats
 * ne prouvent pas automatiquement l integrite en l absence de verification.

 * @par Contrat complementaire initial
 * Retryable worker state in durable repair journal.
 */
enum class RepairState { 
    /**
     * @brief Regeneration proposee et journalisee.
     */
    scheduled,
    /**
     * @brief Worker en cours sur la tache.
     */
    running,
    /**
     * @brief Shard reconstruit persiste avec receipt.
     */
    durable,
    /**
     * @brief Nouvel emplacement signe et publie.
     */
    advertised,
    /**
     * @brief Echec a conserver pour reessai explicite.
     */
    failed 
};

} // namespace ygg::repair
