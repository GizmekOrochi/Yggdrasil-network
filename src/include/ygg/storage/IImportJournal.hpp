#pragma once

/**
 * @file IImportJournal.hpp
 * @brief Journal durable des étapes d import.
 */

#include "ygg/core/Result.hpp"
#include "ygg/storage/Domain.hpp"

namespace ygg::storage {

/**
 * @brief Liste des etats/codes stables de ImportPhase.
 * @details Les valeurs sont contractuelles; les transitions et interpretations doivent etre
 * explicitement validees.
 * @par Responsabilite du fichier Port des transactions d import durables.
 * @par Regles importantes PROTECTED signifie shards, manifeste, catalogue local et sauvegarde
 * distante durable, jamais simple emission de requetes.

 * @par Contrat complementaire initial
 * Durable import lifecycle; protected only after every acknowledgement.
 */
enum class ImportPhase {
    /**
     * @brief Operation preparee, non garantie durable.
     */
    staged, 
    /**
     * @brief Trois fragments par segment confirmes durablement.
     */
    shards_durable, 
    /**
     * @brief Manifeste signe suffisamment replique.
     */
    manifest_durable, 
    /**
     * @brief Catalogue prive local commit dans sa transaction.
     */
    catalog_committed,
    /**
     * @brief Snapshot de recuperation externe confirme durable.
     */
    backup_durable, 
    /**
     * @brief Toutes les garanties documentees sont reunies.
     */
    protected_, 
    /**
     * @brief Reprise necessaire apres interruption.
     */
    pending_recovery, 
    /**
     * @brief Import termine en echec sans succes annonce.
     */
    failed
};

/**
 * @brief Composant IImportJournal du module storage.
 * @details Conserve les transitions successives de l import afin de reprendre ou annuler proprement
 * apres crash.
 * @par Responsabilite du fichier Port des transactions d import durables.
 * @par Regles importantes PROTECTED signifie shards, manifeste, catalogue local et sauvegarde
 * distante durable, jamais simple emission de requetes.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Crash-safe state transitions indexed by stable import reference.
 */
class IImportJournal {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IImportJournal() = default;
    /**
     * @brief Cree durablement l etat staged d une nouvelle version avant tout transfert.
     * @param file Reference stable de la version de fichier concernée.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<void> begin(FileRef file) = 0;
    /**
     * @brief Passe dans la phase demandee apres verification des prerequis de durabilite.
     * @param file Reference stable de la version de fichier concernée.
     * @param phase Argument fourni par l appelant; validite et contraintes definies par ce contrat.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<void> transition(FileRef file, ImportPhase phase) = 0;
    /**
     * @brief Lit l etat persiste actuel d un import donne.
     * @param file Reference stable de la version de fichier concernée.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<ImportPhase> phase(FileRef file) const = 0;
    /**
     * @brief Enumere les imports non finalises pour reprise au demarrage.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<std::vector<FileRef>> incomplete() const = 0;
};
} // namespace ygg::storage
