#pragma once

/**
 * @file SQLitePrivateCatalog.hpp
 * @brief Index SQLite local avec champs sensibles chiffres.
 */

#include "ygg/storage/IPrivateCatalog.hpp"
#include <memory>

namespace ygg::storage {
    
/**
 * @brief Composant SQLitePrivateCatalog du module storage.
 * @details Persiste les entrees du catalogue local dans une transaction et recherche la version
 * precise d un fichier.
 * @par Responsabilite du fichier Adaptateur SQLite des index prives chiffrés.
 * @par Regles importantes Ne pas stocker de nom de fichier clair ni de DEK dechiffree; proteger les
 * operations de lecture concurrrentes.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * SQLite-backed local catalog of already-encrypted private payloads.
 */
class SQLitePrivateCatalog final : public IPrivateCatalog {
public:
    /**
     * @brief Prepare le chemin vers le catalogue SQLite local protege.
     * @param database Chemin de la base SQLite locale.
     */
    explicit SQLitePrivateCatalog(Path database);
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    ~SQLitePrivateCatalog() override;
    /**
     * @brief Persiste transactionnellement les donnees privees deja chiffrees et validees.
     * @param entry Evenement ou entree a persister selon le contexte.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<void> commit(PrivateCatalogEntry entry) override;
    /**
     * @brief Effectue une lecture transactionnelle d une entree du catalogue selon FileRef.
     * @param file Reference stable de la version de fichier concernée.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<PrivateCatalogEntry> lookup(FileRef file) const override;
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
