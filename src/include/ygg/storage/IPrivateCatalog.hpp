#pragma once

/**
 * @file IPrivateCatalog.hpp
 * @brief Port du catalogue prive du proprietaire.
 */

#include "ygg/core/Result.hpp"
#include "ygg/storage/Domain.hpp"

namespace ygg::storage {
    
/**
 * @brief Composant IPrivateCatalog du module storage.
 * @details Associe une reference de fichier, un manifeste et une enveloppe de DEK, avec nom prive
 * chiffre.
 * @par Responsabilite du fichier Port de metadonnees privees cote utilisateur.
 * @par Regles importantes SQLite ne chiffre pas les donnees de lui-meme; chiffrer et authentifier
 * les champs sensibles avant ecriture.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Client-only private index of encrypted file names and wrapped DEKs. Plain SQLite files are not
 * encryption. Sensitive catalog fields must be encrypted and authenticated before persistence.
 */
class IPrivateCatalog {
public:
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    virtual ~IPrivateCatalog() = default;
    /**
     * @brief Insere ou remplace atomiquement une version de catalogue dont tous les champs prives
     * sont chiffres.
     * @param entry Evenement ou entree a persister selon le contexte.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<void> commit(PrivateCatalogEntry entry) = 0;
    /**
     * @brief Retrouve une version precise du fichier, sa reference de manifeste et la DEK
     * enveloppee.
     * @param file Reference stable de la version de fichier concernée.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] virtual Result<PrivateCatalogEntry> lookup(FileRef file) const = 0;
};
} // namespace ygg::storage
