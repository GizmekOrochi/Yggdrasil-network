#pragma once

/**
 * @file IdentityStore.hpp
 * @brief Chargement ou generation locale de la cle de noeud.
 */

#include "ygg/identity/NodeIdentity.hpp"

namespace ygg::identity {
    
/**
 * @brief Composant IdentityStore du module identity.
 * @details Charge l identite stable ou cree une nouvelle paire de cles protegee lors du premier
 * lancement.
 * @par Responsabilite du fichier Persistance locale de l identite de noeud.
 * @par Regles importantes Creer les fichiers atomiquement et avec permissions restrictives; ne
 * jamais regenerer silencieusement une identite corrompue.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Local encrypted/permission-restricted node identity persistence.
 */
class IdentityStore final {
public:
    /**
     * @brief Memorise le chemin prive de stockage de l identite du nœud.
     * @param location Chemin de persistance locale du composant.
     */
    explicit IdentityStore(Path location);

    /**
     * @brief Charge une identite stable protegèe ou la genere une seule fois, de facon atomique et
     * restrictive.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).

 * @par Contrat complementaire initial
 * Load or create the identity without exposing raw private keys.
 */
    [[nodiscard]] Result<NodeIdentity> load_or_create();
private:
    /**
     * @brief Chemin de stockage local de l identite.
     */
    Path location_;
};

} // namespace ygg::identity
