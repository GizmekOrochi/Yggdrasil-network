#pragma once

/**
 * @file SQLiteImportJournal.hpp
 * @brief Journal SQLite des opérations d import.
 */

#include "ygg/storage/IImportJournal.hpp"
#include <memory>

namespace ygg::storage {
    
/**
 * @brief Composant SQLiteImportJournal du module storage.
 * @details Garantit les transitions d etat atomiques et la recherche des imports incomplets apres
 * redemarrage.
 * @par Responsabilite du fichier Adaptateur SQLite du journal d import.
 * @par Regles importantes Regler transaction/synchronous/WAL et traiter les crashs entre les phases
 * pour ne jamais annoncer PROTECTED a tort.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * SQLite transactional import journal; never reports a false success.
 */
class SQLiteImportJournal final : public IImportJournal {
public:
    /**
     * @brief Selectionne la base SQLite du journal d imports, distincte du contenu.
     * @param database Chemin de la base SQLite locale.
     */
    explicit SQLiteImportJournal(Path database);
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    ~SQLiteImportJournal() override;
    /**
     * @brief Enregistre staged dans une transaction durable avant l ecriture des fragments.
     * @param file Reference stable de la version de fichier concernée.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> begin(FileRef file) override;
    /**
     * @brief Controle l ordre des phases et les transitions durables dans SQLite.
     * @param file Reference stable de la version de fichier concernée.
     * @param phase Argument fourni par l appelant; validite et contraintes definies par ce contrat.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> transition(FileRef file, ImportPhase phase) override;
    /**
     * @brief Lit la derniere phase effectivement validee pour un FileRef.
     * @param file Reference stable de la version de fichier concernée.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<ImportPhase> phase(FileRef file) const override;
    /**
     * @brief Recherche toutes les transactions sans statut protected_ final.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<std::vector<FileRef>> incomplete() const override;
private:
    /**
     * @brief Declaration anticipee de State; la definition/etat se trouve dans l implementation.
     */
    struct State;
    /**
     * @brief Etat prive de l adaptateur protege par les invariants RAII.
     */
    std::unique_ptr<State> state_;
};
} // namespace ygg::storage
