#pragma once

/**
 * @file CertificateVerifier.hpp
 * @brief Verification des certificats et pinning des pairs.
 */

#include "ygg/identity/PeerTrustStore.hpp"

namespace ygg::identity {

/**
 * @brief Composant CertificateVerifier du module identity.
 * @details Associe un certificat verifie a un PeerId connu et approuve dans la base de confiance.
 * @par Responsabilite du fichier Verification forte d une identite TLS distante.
 * @par Regles importantes Verifications X.509/Ed25519, periode de validite, identite liee et
 * empreinte epinglee exigées avant de remettre le canal a l application.
 * @par Contrat de duree de vie Les dependances recues par reference restent propriete de leur
 * createur; les ressources internes sont liberees au plus tard au destructeur.

 * @par Contrat complementaire initial
 * Bind a mutually authenticated TLS certificate to an approved PeerId. Implementations must
 * verify signatures, validity constraints and the pinned Ed25519 public key. Disabling
 * certificate verification is forbidden.
 */
class CertificateVerifier final {
public:
    /**
     * @brief Recupere par reference le store d approbation qui doit survivre au verificateur.
     * @param trusted Argument fourni par l appelant; validite et contraintes definies par ce
     * contrat.
     */
    explicit CertificateVerifier(const PeerTrustStore& trusted);
    /**
     * @brief Parse le DER, verifie les proprietes X.509 et la cle liee au PeerId; refuse tout pair
     * inconnu.
     * @param der_certificate Encodage DER du certificat du pair presente en TLS.
     * @return Resultat explicite : valeur valide en succes, ErrorCode type en echec.
     * @warning Un parse DER correct ne remplace pas la validation cryptographique et le pinning
     * exact.
     * @par Erreurs En cas de validation, I/O, connexion ou invariant viole, retourner
     * Result::failure(Error{...}).
     */
    [[nodiscard]] Result<PeerId> verify_certificate(ByteView der_certificate) const;
private:
    /**
     * @brief Store de confiance externe ou reference au verificateur.
     */
    const PeerTrustStore& trust_;
};

} // namespace ygg::identity
