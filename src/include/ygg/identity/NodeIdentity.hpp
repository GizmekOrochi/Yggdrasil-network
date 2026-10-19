#pragma once

/**
 * @file NodeIdentity.hpp
 * @brief Identite Ed25519 persistante et operation de signature.
 */

#include "ygg/core/Ids.hpp"
#include "ygg/core/Result.hpp"
#include "ygg/core/Types.hpp"
#include <memory>

namespace ygg::identity {

/**
 * @brief Identite publique stable et signature privee du noeud.
 * @details La classe est creee par IdentityStore et ne donne jamais acces aux bytes prives.
 * @par Responsabilite du fichier Identite Ed25519 persistante de la machine.
 * @par Regles importantes Ne pas confondre l ID avec une adresse IP. La cle secrète est possedee
 * et detruite avec l objet RAII.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Persistent signing identity of one node. The secret key must be kept in protected memory by
 * the implementation; only a public key and opaque signing operations appear in this contract.
 */
class NodeIdentity final {
public:
    /**
     * @brief Expose l identifiant de nœud derive de la cle publique selon la convention d identite
     * retenue.
     * @return Identite stable du pair, issue des preuves cryptographiques.
     */
    [[nodiscard]] PeerId node_id() const noexcept;
    /**
     * @brief Retourne la cle publique Ed25519 necessaire a l approbation et aux signatures.
     * @return Copie de la cle publique; jamais la cle privee.
     */
    [[nodiscard]] PublicKey32 public_key() const noexcept;
    /**
     * @brief Produit une signature Ed25519 des octets fournis sans exposer la cle privee.
     * @param message Message en octets a signer ou a transmettre.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<Signature> sign(ByteView message) const;
    /**
     * @brief Genere ou retourne un certificat DER liant correctement la cle TLS a l identite du
     * noeud.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @attention La gestion d expiration/renouvellement du certificat doit preserver l identite de
     * noeud stable.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<Bytes> tls_certificate_der() const;
    /**
     * @brief Transfere ou cree un objet d identite avec possession unique de ses secrets.
     */
    NodeIdentity(NodeIdentity&&) noexcept;
    /**
     * @brief Deplace le secret en liberant de facon sure tout etat precedemment possede.
     */
    NodeIdentity& operator=(NodeIdentity&&) noexcept;
    /**
     * @brief Libere les ressources de cette instance conformement aux regles RAII; ne doit pas
     * propager d exception.
     * @post Les ressources acquises ne sont plus accessibles.
     */
    ~NodeIdentity();
    /**
     * @brief Transfere ou cree un objet d identite avec possession unique de ses secrets.
     */
    NodeIdentity(const NodeIdentity&) = delete;
    /**
     * @brief Deplace le secret en liberant de facon sure tout etat precedemment possede.
     */
    NodeIdentity& operator=(const NodeIdentity&) = delete;
private:
    friend class IdentityStore;
    /**
     * @brief Transfere ou cree un objet d identite avec possession unique de ses secrets.
     */
    NodeIdentity();
    /**
     * @brief Declaration anticipee de SecretState; la definition/etat se trouve dans l
     * implementation.
     */
    struct SecretState;
    /**
     * @brief Etat prive de l adaptateur protege par les invariants RAII.
     */
    std::unique_ptr<SecretState> state_;
};

} // namespace ygg::identity
