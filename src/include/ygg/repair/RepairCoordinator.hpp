#pragma once

/**
 * @file RepairCoordinator.hpp
 * @brief Orchestrateur borne des reparations automatiques.
 */

#include "ygg/repair/AvailabilityScanner.hpp"
#include "ygg/repair/RepairPlanner.hpp"
#include "ygg/repair/RepairWorker.hpp"
#include <cstddef>

namespace ygg::repair {
    
/**
 * @brief Controle la file et le nombre de jobs de reparation.
 * @details La deduplication des jobs doit etre idempotente entre redemarrages.
 * @par Responsabilite du fichier Orchestrateur a capacite bornée des travaux d autoréparation.
 * @par Regles importantes Eviter les tempetes de reparation lors de deconnexions breves; permettre
 * un arret propre et reprendre les jobs interrompus.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Bounded self-healing jobs and conflict/idempotency coordination.
 */
class RepairCoordinator final {
public:
    /**
     * @brief Injecte analyseur, planificateur et worker; borne le nombre de jobs simultanes.
     * @note Les references injectees ne sont pas possedees; leurs objets doivent survivre a cette
     * instance.
     * @param scanner Argument fourni par l appelant; validite et contraintes definies par ce
     * contrat.
     * @param planner Argument fourni par l appelant; validite et contraintes definies par ce
     * contrat.
     * @param worker Argument fourni par l appelant; validite et contraintes definies par ce
     * contrat.
     * @param max_jobs Nombre maximal de jobs autorises simultanement.
     */
    RepairCoordinator(AvailabilityScanner& scanner, RepairPlanner& planner, RepairWorker& worker, std::size_t max_jobs);
    /**
     * @brief Ajoute un plan non deja programme, sous quota et avec cle d idempotence.
     * @param plan Instructions de reconstruction deja validees.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> schedule(RepairPlan plan);
    /**
     * @brief Execute les jobs admissibles en respectant quota, delai de grace et journal durable.
     * @return Coroutine retournant Result<T> ; les echecs reseau/durabilite sont representes par
     * Error.
     * @note Le squelette n expose pas encore le stockage des plans en attente; une queue bornee et
     * un journal seront necessaires.
     * @note Coroutine : aucun verrou bloquant ni vue ByteView empruntee ne doit traverser un
     * co_await.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Async<void> run_pending();
    /**
     * @brief Annule/termine les jobs au shutdown sans perdre leur progression persistante.
     */
    void stop() noexcept;
private:
    /**
     * @brief Analyseur externe de la disponibilite des shards.
     */
    AvailabilityScanner& scanner_;
    /**
     * @brief Planificateur externe des reparations.
     */
    RepairPlanner& planner_;
    /**
     * @brief Executant externe de jobs de regeneration.
     */
    RepairWorker& worker_;
    /**
     * @brief Nombre maximum de travaux actifs ou planifies en parallele.
     */
    std::size_t max_jobs_{};
};
} // namespace ygg::repair
