/*
 * moves.c - Move generation and checking
 *
 * Copyright 1991 by Digital Equipment Corporation, Maynard,
 * Massachusetts.
 *
 * Enhancements Copyright 1992-2001, 2002, 2003, 2004, 2005, 2006,
 * 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
 *
 * Enhancements Copyright 2005 Alessandro Scotti
 *
 * The following terms apply to Digital Equipment Corporation's copyright
 * interest in XBoard:
 * ------------------------------------------------------------------------
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * ------------------------------------------------------------------------
 *
 * The following terms apply to the enhanced version of XBoard
 * distributed by the Free Software Foundation:
 * ------------------------------------------------------------------------
 *
 * GNU XBoard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * GNU XBoard is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.  *
 *
 *------------------------------------------------------------------------
 ** See the file ChangeLog for a revision history.  */

#include "config.h"

#include <stdio.h>
#if HAVE_STRING_H
# include <string.h>
#else /* not HAVE_STRING_H */
# include <strings.h>
#endif /* not HAVE_STRING_H */
#include "common.h"
#include "backend.h" 
#include "moves.h"
#include "parser.h"

int WhitePiece P((ChessSquare));
int BlackPiece P((ChessSquare));
int SameColor P((ChessSquare, ChessSquare));
int PosFlags(int index);

extern signed char initialRights[BOARD_SIZE]; /* [HGM] all rights enabled, set in InitPosition */


int WhitePiece(piece)
     ChessSquare piece;
{
    return (int) piece >= (int) WhitePawn && (int) piece < (int) BlackPawn;
}

int BlackPiece(piece)
     ChessSquare piece;
{
    return (int) piece >= (int) BlackPawn && (int) piece < (int) EmptySquare;
}

int SameColor(piece1, piece2)
     ChessSquare piece1, piece2;
{
    return ((int) piece1 >= (int) WhitePawn &&   /* [HGM] can be > King ! */
            (int) piece1 <  (int) BlackPawn &&
	    (int) piece2 >= (int) WhitePawn &&
            (int) piece2 <  (int) BlackPawn)
      ||   ((int) piece1 >= (int) BlackPawn &&
            (int) piece1 <  (int) EmptySquare &&
	    (int) piece2 >= (int) BlackPawn &&
            (int) piece2 <  (int) EmptySquare);
}

ChessSquare PromoPiece(moveType)
     ChessMove moveType;
{
    switch (moveType) {
      default:
	return EmptySquare;
      case WhitePromotionQueen:
	return WhiteQueen;
      case BlackPromotionQueen:
	return BlackQueen;
      case WhitePromotionRook:
	return WhiteRook;
      case BlackPromotionRook:
	return BlackRook;
      case WhitePromotionBishop:
	return WhiteBishop;
      case BlackPromotionBishop:
	return BlackBishop;
      case WhitePromotionKnight:
	return WhiteKnight;
      case BlackPromotionKnight:
	return BlackKnight;
      case WhitePromotionKing:
	return WhiteKing;
      case BlackPromotionKing:
	return BlackKing;
      case WhitePromotionChancellor:
        return WhiteMarshall;
      case BlackPromotionChancellor:
        return BlackMarshall;
      case WhitePromotionArchbishop:
        return WhiteAngel;
      case BlackPromotionArchbishop:
        return BlackAngel;
      case WhitePromotionCentaur:
        return WhiteSilver;
      case BlackPromotionCentaur:
        return BlackSilver;
    }
}

char pieceToChar[] = {
                        'P', 'N', 'B', 'R', 'Q', 'F', 'E', 'A', 'C', 'W', 'M', 
                        'O', 'H', 'I', 'J', 'G', 'D', 'V', 'L', 'S', 'U', 'K',
                        'p', 'n', 'b', 'r', 'q', 'f', 'e', 'a', 'c', 'w', 'm', 
                        'o', 'h', 'i', 'j', 'g', 'd', 'v', 'l', 's', 'u', 'k', 
                        'x' };

char PieceToChar(p)
     ChessSquare p;
{
    if((int)p < 0 || (int)p >= (int)EmptySquare) return('x'); /* [HGM] for safety */
    return pieceToChar[(int) p];
}

int PieceToNumber(p)  /* [HGM] holdings: count piece type, ignoring non-participating piece types */
     ChessSquare p;
{
    int i=0;
    ChessSquare start = (int)p >= (int)BlackPawn ? BlackPawn : WhitePawn;

    while(start++ != p) if(pieceToChar[(int)start-1] != '.') i++;
    return i;
}

ChessSquare CharToPiece(c)
     int c;
{
     int i;
     for(i=0; i< (int) EmptySquare; i++)
          if(pieceToChar[i] == c) return (ChessSquare) i;
     return EmptySquare;
}

ChessMove PromoCharToMoveType(whiteOnMove, promoChar)
     int whiteOnMove;
     int promoChar;
{	/* [HGM] made dependent on CharToPiece to alow alternate piece letters */
	ChessSquare piece = CharToPiece(whiteOnMove ? ToUpper(promoChar) : ToLower(promoChar) );


	if(promoChar == NULLCHAR) return NormalMove;

	switch(piece) {
		case WhiteQueen:
			return WhitePromotionQueen;
		case WhiteRook:
			return WhitePromotionRook;
		case WhiteBishop:
			return WhitePromotionBishop;
		case WhiteKnight:
			return WhitePromotionKnight;
		case WhiteKing:
			return WhitePromotionKing;
		case WhiteAngel:
			return WhitePromotionArchbishop;
		case WhiteMarshall:
			return WhitePromotionChancellor;
		case WhiteSilver:
			return WhitePromotionCentaur;
		case BlackQueen:
			return BlackPromotionQueen;
		case BlackRook:
			return BlackPromotionRook;
		case BlackBishop:
			return BlackPromotionBishop;
		case BlackKnight:
			return BlackPromotionKnight;
		case BlackKing:
			return BlackPromotionKing;
		case BlackAngel:
			return BlackPromotionArchbishop;
		case BlackMarshall:
			return BlackPromotionChancellor;
		case BlackSilver:
			return BlackPromotionCentaur;
		default:
			// not all promotion implemented yet! Take Queen for those we don't know.
			return (whiteOnMove ? WhitePromotionQueen : BlackPromotionQueen);
	}
}

void CopyBoard(to, from)
     Board to, from;
{
    int i, j;
    
    for (i = 0; i < BOARD_HEIGHT; i++)
      for (j = 0; j < BOARD_WIDTH; j++)
	to[i][j] = from[i][j];
}

int CompareBoards(board1, board2)
     Board board1, board2;
{
    int i, j;
    
    for (i = 0; i < BOARD_HEIGHT; i++)
      for (j = 0; j < BOARD_WIDTH; j++) {
	  if (board1[i][j] != board2[i][j])
	    return FALSE;
    }
    return TRUE;
}


/* Call callback once for each pseudo-legal move in the given
   position, except castling moves. A move is pseudo-legal if it is
   legal, or if it would be legal except that it leaves the king in
   check.  In the arguments, epfile is EP_NONE if the previous move
   was not a double pawn push, or the file 0..7 if it was, or
   EP_UNKNOWN if we don't know and want to allow all e.p. captures.
   Promotion moves generated are to Queen only.
*/
void GenPseudoLegal(board, flags, epfile, callback, closure)
     Board board;
     int flags;
     int epfile;
     MoveCallback callback;
     VOIDSTAR closure;
{
    int rf, ff;
    int i, j, d, s, fs, rs, rt, ft, m;

    for (rf = 0; rf < BOARD_HEIGHT; rf++) 
      for (ff = BOARD_LEFT; ff < BOARD_RGHT; ff++) {
          ChessSquare piece;

	  if (flags & F_WHITE_ON_MOVE) {
	      if (!WhitePiece(board[rf][ff])) continue;
	  } else {
	      if (!BlackPiece(board[rf][ff])) continue;
	  }
          m = 0; piece = board[rf][ff];
          if(PieceToChar(piece) == '~') 
                 piece = (ChessSquare) ( DEMOTED piece );
          if(gameInfo.variant == VariantShogi)
                 piece = (ChessSquare) ( SHOGI piece );

          switch (piece) {
            /* case EmptySquare: [HGM] this is nonsense, and conflicts with Shogi cases */
	    default:
	      /* can't happen ([HGM] except for faries...) */
	      break;

             case WhitePawn:
              if(gameInfo.variant == VariantXiangqi) {
                  /* [HGM] capture and move straight ahead in Xiangqi */
                  if (rf < BOARD_HEIGHT-1 &&
                           !SameColor(board[rf][ff], board[rf + 1][ff]) ) {
                           callback(board, flags, NormalMove,
                                    rf, ff, rf + 1, ff, closure);
                  }
                  /* and move sideways when across the river */
                  for (s = -1; s <= 1; s += 2) {
                      if (rf >= BOARD_HEIGHT>>1 &&
                          ff + s >= BOARD_LEFT && ff + s < BOARD_RGHT &&
                          !WhitePiece(board[rf][ff+s]) ) {
                           callback(board, flags, NormalMove,
                                    rf, ff, rf, ff+s, closure);
                      }
                  }
                  break;
              }
              if (rf < BOARD_HEIGHT-1 && board[rf + 1][ff] == EmptySquare) {
		  callback(board, flags,
			   rf == BOARD_HEIGHT-2 ? WhitePromotionQueen : NormalMove,
			   rf, ff, rf + 1, ff, closure);
	      }
	      if (rf == 1 && board[2][ff] == EmptySquare &&
                  gameInfo.variant != VariantShatranj && /* [HGM] */
                  gameInfo.variant != VariantCourier  && /* [HGM] */
                  board[3][ff] == EmptySquare ) {
                      callback(board, flags, NormalMove,
                               rf, ff, 3, ff, closure);
	      }
	      for (s = -1; s <= 1; s += 2) {
                  if (rf < BOARD_HEIGHT-1 && ff + s >= BOARD_LEFT && ff + s < BOARD_RGHT &&
		      ((flags & F_KRIEGSPIEL_CAPTURE) ||
		       BlackPiece(board[rf + 1][ff + s]))) {
		      callback(board, flags, 
			       rf == BOARD_HEIGHT-2 ? WhitePromotionQueen : NormalMove,
			       rf, ff, rf + 1, ff + s, closure);
		  }
		  if (rf == BOARD_HEIGHT-4) {
                      if (ff + s >= BOARD_LEFT && ff + s < BOARD_RGHT &&
			  (epfile == ff + s || epfile == EP_UNKNOWN) &&
                          board[BOARD_HEIGHT-4][ff + s] == BlackPawn &&
                          board[BOARD_HEIGHT-3][ff + s] == EmptySquare) {
			  callback(board, flags, WhiteCapturesEnPassant,
				   rf, ff, 5, ff + s, closure);
		      }
		  }
	      }		    
	      break;

	    case BlackPawn:
              if(gameInfo.variant == VariantXiangqi) {
                  /* [HGM] capture straight ahead in Xiangqi */
                  if (rf > 0 && !SameColor(board[rf][ff], board[rf - 1][ff]) ) {
                           callback(board, flags, NormalMove,
                                    rf, ff, rf - 1, ff, closure);
                  }
                  /* and move sideways when across the river */
                  for (s = -1; s <= 1; s += 2) {
                      if (rf < BOARD_HEIGHT>>1 &&
                          ff + s >= BOARD_LEFT && ff + s < BOARD_RGHT &&
                          !BlackPiece(board[rf][ff+s]) ) {
                           callback(board, flags, NormalMove,
                                    rf, ff, rf, ff+s, closure);
                      }
                  }
                  break;
              }
	      if (rf > 0 && board[rf - 1][ff] == EmptySquare) {
		  callback(board, flags, 
			   rf == 1 ? BlackPromotionQueen : NormalMove,
			   rf, ff, rf - 1, ff, closure);
	      }
	      if (rf == BOARD_HEIGHT-2 && board[BOARD_HEIGHT-3][ff] == EmptySquare &&
                  gameInfo.variant != VariantShatranj && /* [HGM] */
                  gameInfo.variant != VariantCourier  && /* [HGM] */
		  board[BOARD_HEIGHT-4][ff] == EmptySquare) {
		  callback(board, flags, NormalMove,
			   rf, ff, BOARD_HEIGHT-4, ff, closure);
	      }
	      for (s = -1; s <= 1; s += 2) {
                  if (rf > 0 && ff + s >= BOARD_LEFT && ff + s < BOARD_RGHT &&
		      ((flags & F_KRIEGSPIEL_CAPTURE) ||
		       WhitePiece(board[rf - 1][ff + s]))) {
		      callback(board, flags, 
			       rf == 1 ? BlackPromotionQueen : NormalMove,
			       rf, ff, rf - 1, ff + s, closure);
		  }
		  if (rf == 3) {
                      if (ff + s >= BOARD_LEFT && ff + s < BOARD_RGHT &&
			  (epfile == ff + s || epfile == EP_UNKNOWN) &&
			  board[3][ff + s] == WhitePawn &&
			  board[2][ff + s] == EmptySquare) {
			  callback(board, flags, BlackCapturesEnPassant,
				   rf, ff, 2, ff + s, closure);
		      }
		  }
	      }		    
	      break;

            case WhiteUnicorn:
            case BlackUnicorn:
	    case WhiteKnight:
	    case BlackKnight:
            mounted:
	      for (i = -1; i <= 1; i += 2)
		for (j = -1; j <= 1; j += 2)
		  for (s = 1; s <= 2; s++) {
		      rt = rf + i*s;
		      ft = ff + j*(3-s);
                      if (!(rt < 0 || rt >= BOARD_HEIGHT || ft < BOARD_LEFT || ft >= BOARD_RGHT)
                          && ( gameInfo.variant != VariantXiangqi || board[rf+i*(s-1)][ff+j*(2-s)] == EmptySquare)
                          && !SameColor(board[rf][ff], board[rt][ft]))
		      callback(board, flags, NormalMove,
			       rf, ff, rt, ft, closure);
		  }
	      break;

            case SHOGI WhiteKnight:
	      for (s = -1; s <= 1; s += 2) {
                  if (rf < BOARD_HEIGHT-2 && ff + s >= BOARD_LEFT && ff + s < BOARD_RGHT &&
                      !SameColor(board[rf][ff], board[rf + 2][ff + s])) {
                      callback(board, flags, NormalMove,
                               rf, ff, rf + 2, ff + s, closure);
		  }
              }
	      break;

            case SHOGI BlackKnight:
	      for (s = -1; s <= 1; s += 2) {
                  if (rf > 1 && ff + s >= BOARD_LEFT && ff + s < BOARD_RGHT &&
                      !SameColor(board[rf][ff], board[rf - 2][ff + s])) {
                      callback(board, flags, NormalMove,
                               rf, ff, rf - 2, ff + s, closure);
		  }
	      }		    
	      break;

            case WhiteCannon:
            case BlackCannon:
              for (d = 0; d <= 1; d++)
                for (s = -1; s <= 1; s += 2) {
                  m = 0;
		  for (i = 1;; i++) {
		      rt = rf + (i * s) * d;
		      ft = ff + (i * s) * (1 - d);
                      if (rt < 0 || rt >= BOARD_HEIGHT || ft < BOARD_LEFT || ft >= BOARD_RGHT) break;
                      if (m == 0 && board[rt][ft] == EmptySquare)
                                 callback(board, flags, NormalMove,
                                          rf, ff, rt, ft, closure);
                      if (m == 1 && board[rt][ft] != EmptySquare &&
                          !SameColor(board[rf][ff], board[rt][ft]) )
                                 callback(board, flags, NormalMove,
                                          rf, ff, rt, ft, closure);
                      if (board[rt][ft] != EmptySquare && m++) break;
                  }
                }
	      break;

            /* Gold General (and all its promoted versions) . First do the */
            /* diagonal forward steps, then proceed as normal Wazir        */
            case SHOGI WhiteWazir:
            case SHOGI (PROMOTED WhitePawn):
            case SHOGI (PROMOTED WhiteKnight):
            case SHOGI (PROMOTED WhiteQueen):
            case SHOGI (PROMOTED WhiteFerz):
	      for (s = -1; s <= 1; s += 2) {
                  if (rf < BOARD_HEIGHT-1 && ff + s >= BOARD_LEFT && ff + s < BOARD_RGHT &&
                      !SameColor(board[rf][ff], board[rf + 1][ff + s])) {
                      callback(board, flags, NormalMove,
			       rf, ff, rf + 1, ff + s, closure);
		  }
              }
              goto finishGold;

            case SHOGI BlackWazir:
            case SHOGI (PROMOTED BlackPawn):
            case SHOGI (PROMOTED BlackKnight):
            case SHOGI (PROMOTED BlackQueen):
            case SHOGI (PROMOTED BlackFerz):
	      for (s = -1; s <= 1; s += 2) {
                  if (rf > 0 && ff + s >= BOARD_LEFT && ff + s < BOARD_RGHT &&
                      !SameColor(board[rf][ff], board[rf - 1][ff + s])) {
                      callback(board, flags, NormalMove,
			       rf, ff, rf - 1, ff + s, closure);
		  }
	      }		    

            case WhiteWazir:
            case BlackWazir:
            finishGold:
              for (d = 0; d <= 1; d++)
                for (s = -1; s <= 1; s += 2) {
                      rt = rf + s * d;
                      ft = ff + s * (1 - d);
                      if (!(rt < 0 || rt >= BOARD_HEIGHT || ft < BOARD_LEFT || ft >= BOARD_RGHT)
                          && !SameColor(board[rf][ff], board[rt][ft]) &&
                          (gameInfo.variant != VariantXiangqi || InPalace(rt, ft) ) )
                               callback(board, flags, NormalMove,
                                        rf, ff, rt, ft, closure);
                      }
	      break;

            case WhiteAlfil:
            case BlackAlfil:
                /* [HGM] support Shatranj pieces */
                for (rs = -1; rs <= 1; rs += 2) 
                  for (fs = -1; fs <= 1; fs += 2) {
                      rt = rf + 2 * rs;
                      ft = ff + 2 * fs;
                      if (!(rt < 0 || rt >= BOARD_HEIGHT || ft < BOARD_LEFT || ft >= BOARD_RGHT)
                          && ( gameInfo.variant != VariantXiangqi ||
                               board[rf+rs][ff+fs] == EmptySquare && (2*rf < BOARD_HEIGHT) == (2*rt < BOARD_HEIGHT) )
                         
                          && !SameColor(board[rf][ff], board[rt][ft]))
                               callback(board, flags, NormalMove,
                                        rf, ff, rt, ft, closure);
		  }
                break;

            /* Shogi Dragon Horse has to continue with Wazir after Bishop */
            case SHOGI WhiteCardinal:
            case SHOGI BlackCardinal:
              m++;

            /* Capablanca Archbishop continues as Knight                  */
            case WhiteAngel:
            case BlackAngel:
              m++;

            /* Shogi Bishops are ordinary Bishops */
            case SHOGI WhiteBishop:
            case SHOGI BlackBishop:
	    case WhiteBishop:
	    case BlackBishop:
	      for (rs = -1; rs <= 1; rs += 2) 
                for (fs = -1; fs <= 1; fs += 2) 
		  for (i = 1;; i++) {
		      rt = rf + (i * rs);
		      ft = ff + (i * fs);
                      if (rt < 0 || rt >= BOARD_HEIGHT || ft < BOARD_LEFT || ft >= BOARD_RGHT) break;
		      if (SameColor(board[rf][ff], board[rt][ft])) break;
		      callback(board, flags, NormalMove,
			       rf, ff, rt, ft, closure);
		      if (board[rt][ft] != EmptySquare) break;
		  }
                if(m==1) goto mounted;
                if(m==2) goto finishGold;
                /* Bishop falls through */
	      break;

            /* Shogi Lance is unlike anything, and asymmetric at that */
            case SHOGI WhiteQueen:
              for(i = 1;; i++) {
                      rt = rf + i;
                      ft = ff;
                      if (rt >= BOARD_HEIGHT) break;
		      if (SameColor(board[rf][ff], board[rt][ft])) break;
		      callback(board, flags, NormalMove,
			       rf, ff, rt, ft, closure);
                      if (board[rt][ft] != EmptySquare) break;
              }
              break;

            case SHOGI BlackQueen:
              for(i = 1;; i++) {
                      rt = rf - i;
                      ft = ff;
                      if (rt < 0) break;
		      if (SameColor(board[rf][ff], board[rt][ft])) break;
		      callback(board, flags, NormalMove,
			       rf, ff, rt, ft, closure);
                      if (board[rt][ft] != EmptySquare) break;
              }
              break;

            /* Shogi Dragon King has to continue as Ferz after Rook moves */
            case SHOGI WhiteDragon:
            case SHOGI BlackDragon:
              m++;

            /* Capablanca Chancellor sets flag to continue as Knight      */
            case WhiteMarshall:
            case BlackMarshall:
              m++;

            /* Shogi Rooks are ordinary Rooks */
            case SHOGI WhiteRook:
            case SHOGI BlackRook:
	    case WhiteRook:
	    case BlackRook:
              for (d = 0; d <= 1; d++)
                for (s = -1; s <= 1; s += 2)
		  for (i = 1;; i++) {
		      rt = rf + (i * s) * d;
		      ft = ff + (i * s) * (1 - d);
                      if (rt < 0 || rt >= BOARD_HEIGHT || ft < BOARD_LEFT || ft >= BOARD_RGHT) break;
		      if (SameColor(board[rf][ff], board[rt][ft])) break;
		      callback(board, flags, NormalMove,
			       rf, ff, rt, ft, closure);
		      if (board[rt][ft] != EmptySquare) break;
		  }
                if(m==1) goto mounted;
                if(m==2) goto finishSilver;
	      break;

	    case WhiteQueen:
	    case BlackQueen:
	      for (rs = -1; rs <= 1; rs++) 
		for (fs = -1; fs <= 1; fs++) {
		    if (rs == 0 && fs == 0) continue;
		    for (i = 1;; i++) {
			rt = rf + (i * rs);
			ft = ff + (i * fs);
                        if (rt < 0 || rt >= BOARD_HEIGHT || ft < BOARD_LEFT || ft >= BOARD_RGHT) break;
			if (SameColor(board[rf][ff], board[rt][ft])) break;
			callback(board, flags, NormalMove,
				 rf, ff, rt, ft, closure);
			if (board[rt][ft] != EmptySquare) break;
		    }
		}
	      break;

            /* Shogi Pawn and Silver General: first the Pawn move,    */
            /* then the General continues like a Ferz                 */
            case SHOGI WhitePawn:
            case SHOGI WhiteFerz:
                  if (rf < BOARD_HEIGHT-1 &&
                           !SameColor(board[rf][ff], board[rf + 1][ff]) ) 
                           callback(board, flags, NormalMove,
                                    rf, ff, rf + 1, ff, closure);
              if(piece != SHOGI WhitePawn) goto finishSilver;
              break;

            case SHOGI BlackPawn:
            case SHOGI BlackFerz:
                  if (rf > 0 &&
                           !SameColor(board[rf][ff], board[rf - 1][ff]) ) 
                           callback(board, flags, NormalMove,
                                    rf, ff, rf - 1, ff, closure);
              if(piece == SHOGI BlackPawn) break;

            case WhiteFerz:
            case BlackFerz:
            finishSilver:
                /* [HGM] support Shatranj pieces */
                for (rs = -1; rs <= 1; rs += 2) 
                  for (fs = -1; fs <= 1; fs += 2) {
                      rt = rf + rs;
                      ft = ff + fs;
                      if (rt < 0 || rt >= BOARD_HEIGHT || ft < BOARD_LEFT || ft >= BOARD_RGHT) continue;
                      if (!SameColor(board[rf][ff], board[rt][ft]) &&
                          (gameInfo.variant != VariantXiangqi || InPalace(rt, ft) ) )
                               callback(board, flags, NormalMove,
                                        rf, ff, rt, ft, closure);
		  }
                break;

	    case WhiteSilver:
	    case BlackSilver:
		m++; // [HGM] superchess: use for Centaur
            case WhiteMan:
            case BlackMan:
            case SHOGI WhiteKing:
            case SHOGI BlackKing:
	    case WhiteKing:
	    case BlackKing:
//            walking:
	      for (i = -1; i <= 1; i++)
		for (j = -1; j <= 1; j++) {
		    if (i == 0 && j == 0) continue;
		    rt = rf + i;
		    ft = ff + j;
                    if (rt < 0 || rt >= BOARD_HEIGHT || ft < BOARD_LEFT || ft >= BOARD_RGHT) continue;
		    if (SameColor(board[rf][ff], board[rt][ft])) continue;
		    callback(board, flags, NormalMove,
			     rf, ff, rt, ft, closure);
		}
		if(m==1) goto mounted;
	      break;

	    case WhiteNightrider:
	    case BlackNightrider:
	      for (i = -1; i <= 1; i += 2)
		for (j = -1; j <= 1; j += 2)
		  for (s = 1; s <= 2; s++) {  int k;
                    for(k=1;; k++) {
		      rt = rf + k*i*s;
		      ft = ff + k*j*(3-s);
                      if (rt < 0 || rt >= BOARD_HEIGHT || ft < BOARD_LEFT || ft >= BOARD_RGHT) break;
		      if (SameColor(board[rf][ff], board[rt][ft])) break;
		      callback(board, flags, NormalMove,
			       rf, ff, rt, ft, closure);
		      if (board[rt][ft] != EmptySquare) break;
                    }
		  }
	      break;
	    case WhiteFalcon: // [HGM] wild: for wildcards, self-capture symbolizes move to anywhere
	    case BlackFalcon:
	    case WhiteCobra:
	    case BlackCobra:
	    case WhiteLance:
	    case BlackLance:
	      callback(board, flags, NormalMove, rf, ff, rf, ff, closure);
	      break;

	  }
      }
}


typedef struct {
    MoveCallback cb;
    VOIDSTAR cl;
} GenLegalClosure;

extern void GenLegalCallback P((Board board, int flags, ChessMove kind,
				int rf, int ff, int rt, int ft,
				VOIDSTAR closure));

void GenLegalCallback(board, flags, kind, rf, ff, rt, ft, closure)
     Board board;
     int flags;
     ChessMove kind;
     int rf, ff, rt, ft;
     VOIDSTAR closure;
{
    register GenLegalClosure *cl = (GenLegalClosure *) closure;

    if (!(flags & F_IGNORE_CHECK) &&
	CheckTest(board, flags, rf, ff, rt, ft,
		  kind == WhiteCapturesEnPassant ||
		  kind == BlackCapturesEnPassant)) return;
    if (flags & F_ATOMIC_CAPTURE) {
      if (board[rt][ft] != EmptySquare ||
	  kind == WhiteCapturesEnPassant || kind == BlackCapturesEnPassant) {
	int r, f;
	ChessSquare king = (flags & F_WHITE_ON_MOVE) ? WhiteKing : BlackKing;
	if (board[rf][ff] == king) return;
	for (r = rt-1; r <= rt+1; r++) {
	  for (f = ft-1; f <= ft+1; f++) {
            if (r >= 0 && r < BOARD_HEIGHT && f >= BOARD_LEFT && f < BOARD_RGHT &&
		board[r][f] == king) return;
	  }
	}
      }
    }
    cl->cb(board, flags, kind, rf, ff, rt, ft, cl->cl);
}


typedef struct {
    int rf, ff, rt, ft;
    ChessMove kind;
    int captures; // [HGM] losers
} LegalityTestClosure;


/* Like GenPseudoLegal, but (1) include castling moves, (2) unless
   F_IGNORE_CHECK is set in the flags, omit moves that would leave the
   king in check, and (3) if F_ATOMIC_CAPTURE is set in the flags, omit
   moves that would destroy your own king.  The CASTLE_OK flags are
   true if castling is not yet ruled out by a move of the king or
   rook.  Return TRUE if the player on move is currently in check and
   F_IGNORE_CHECK is not set.  [HGM] add castlingRights parameter */
int GenLegal(board, flags, epfile, castlingRights, callback, closure)
     Board board;
     int flags;
     int epfile;
     char castlingRights[];
     MoveCallback callback;
     VOIDSTAR closure;
{
    GenLegalClosure cl;
    int ff, ft, k, left, right;
    int ignoreCheck = (flags & F_IGNORE_CHECK) != 0;
    ChessSquare wKing = WhiteKing, bKing = BlackKing;

    cl.cb = callback;
    cl.cl = closure;
    GenPseudoLegal(board, flags, epfile, GenLegalCallback, (VOIDSTAR) &cl);

    if (!ignoreCheck &&
	CheckTest(board, flags, -1, -1, -1, -1, FALSE)) return TRUE;

    /* Generate castling moves */
    if(gameInfo.variant == VariantKnightmate) { /* [HGM] Knightmate */
        wKing = WhiteUnicorn; bKing = BlackUnicorn;
    }

    for (ff = BOARD_WIDTH>>1; ff >= (BOARD_WIDTH-1)>>1; ff-- /*ics wild 1*/) {
	if ((flags & F_WHITE_ON_MOVE) &&
	    (flags & F_WHITE_KCASTLE_OK) &&
            board[0][ff] == wKing &&
            board[0][ff + 1] == EmptySquare &&
            board[0][ff + 2] == EmptySquare &&
            board[0][BOARD_RGHT-3] == EmptySquare &&
            board[0][BOARD_RGHT-2] == EmptySquare &&
            board[0][BOARD_RGHT-1] == WhiteRook &&
            castlingRights[0] >= 0 && /* [HGM] check rights */
            ( castlingRights[2] == ff || castlingRights[6] == ff ) &&
            (ignoreCheck ||                             
	     (!CheckTest(board, flags, 0, ff, 0, ff + 1, FALSE) &&
              !CheckTest(board, flags, 0, ff, 0, BOARD_RGHT-3, FALSE) &&
              (gameInfo.variant != VariantJanus || !CheckTest(board, flags, 0, ff, 0, BOARD_RGHT-2, FALSE)) &&
	      !CheckTest(board, flags, 0, ff, 0, ff + 2, FALSE)))) {

	    callback(board, flags,
                     ff==BOARD_WIDTH>>1 ? WhiteKingSideCastle : WhiteKingSideCastleWild,
                     0, ff, 0, ff + ((gameInfo.boardWidth+2)>>2) + (gameInfo.variant == VariantJanus), closure);
	}
	if ((flags & F_WHITE_ON_MOVE) &&
	    (flags & F_WHITE_QCASTLE_OK) &&
            board[0][ff] == wKing &&
	    board[0][ff - 1] == EmptySquare &&
	    board[0][ff - 2] == EmptySquare &&
            board[0][BOARD_LEFT+2] == EmptySquare &&
            board[0][BOARD_LEFT+1] == EmptySquare &&
            board[0][BOARD_LEFT+0] == WhiteRook &&
            castlingRights[1] >= 0 && /* [HGM] check rights */
            ( castlingRights[2] == ff || castlingRights[6] == ff ) &&
	    (ignoreCheck ||
	     (!CheckTest(board, flags, 0, ff, 0, ff - 1, FALSE) &&
              !CheckTest(board, flags, 0, ff, 0, BOARD_LEFT+3, FALSE) &&
	      !CheckTest(board, flags, 0, ff, 0, ff - 2, FALSE)))) {

	    callback(board, flags,
		     ff==BOARD_WIDTH>>1 ? WhiteQueenSideCastle : WhiteQueenSideCastleWild,
                     0, ff, 0, ff - ((gameInfo.boardWidth+2)>>2), closure);
	}
	if (!(flags & F_WHITE_ON_MOVE) &&
	    (flags & F_BLACK_KCASTLE_OK) &&
            board[BOARD_HEIGHT-1][ff] == bKing &&
	    board[BOARD_HEIGHT-1][ff + 1] == EmptySquare &&
	    board[BOARD_HEIGHT-1][ff + 2] == EmptySquare &&
            board[BOARD_HEIGHT-1][BOARD_RGHT-3] == EmptySquare &&
            board[BOARD_HEIGHT-1][BOARD_RGHT-2] == EmptySquare &&
            board[BOARD_HEIGHT-1][BOARD_RGHT-1] == BlackRook &&
            castlingRights[3] >= 0 && /* [HGM] check rights */
            ( castlingRights[5] == ff || castlingRights[7] == ff ) &&
	    (ignoreCheck ||
	     (!CheckTest(board, flags, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, ff + 1, FALSE) &&
              !CheckTest(board, flags, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, BOARD_RGHT-3, FALSE) &&
              (gameInfo.variant != VariantJanus || !CheckTest(board, flags, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, BOARD_RGHT-2, FALSE)) &&
	      !CheckTest(board, flags, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, ff + 2, FALSE)))) {

	    callback(board, flags,
		     ff==BOARD_WIDTH>>1 ? BlackKingSideCastle : BlackKingSideCastleWild,
                     BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, ff + ((gameInfo.boardWidth+2)>>2) + (gameInfo.variant == VariantJanus), closure);
	}
	if (!(flags & F_WHITE_ON_MOVE) &&
	    (flags & F_BLACK_QCASTLE_OK) &&
            board[BOARD_HEIGHT-1][ff] == bKing &&
	    board[BOARD_HEIGHT-1][ff - 1] == EmptySquare &&
	    board[BOARD_HEIGHT-1][ff - 2] == EmptySquare &&
            board[BOARD_HEIGHT-1][BOARD_LEFT+2] == EmptySquare &&
            board[BOARD_HEIGHT-1][BOARD_LEFT+1] == EmptySquare &&
            board[BOARD_HEIGHT-1][BOARD_LEFT+0] == BlackRook &&
            castlingRights[4] >= 0 && /* [HGM] check rights */
            ( castlingRights[5] == ff || castlingRights[7] == ff ) &&
	    (ignoreCheck ||
	     (!CheckTest(board, flags, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, ff - 1, FALSE) &&
              !CheckTest(board, flags, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, BOARD_LEFT+3, FALSE) &&
              !CheckTest(board, flags, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, ff - 2, FALSE)))) {

	    callback(board, flags,
		     ff==BOARD_WIDTH>>1 ? BlackQueenSideCastle : BlackQueenSideCastleWild,
                     BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, ff - ((gameInfo.boardWidth+2)>>2), closure);
	}
    }

  if(flags & F_FRC_TYPE_CASTLING) {

    /* generate all potential FRC castling moves (KxR), ignoring flags */
    /* [HGM] test if the Rooks we find have castling rights */


    if ((flags & F_WHITE_ON_MOVE) != 0) {
        ff = castlingRights[2]; /* King file if we have any rights */
        if(ff > 0 && board[0][ff] == WhiteKing) {
            ft = castlingRights[0]; /* Rook file if we have H-side rights */
            left  = ff+1;
            right = BOARD_RGHT-2;
            if(ff == BOARD_RGHT-2) left = right = ff-1;    /* special case */
            for(k=left; k<=right && ft >= 0; k++) /* first test if blocked */
                if(k != ft && board[0][k] != EmptySquare) ft = -1;
            for(k=left; k<right && ft >= 0; k++) /* then if not checked */
                if(!ignoreCheck && CheckTest(board, flags, 0, ff, 0, k, FALSE)) ft = -1;
            if(ft >= 0 && board[0][ft] == WhiteRook)
                callback(board, flags, WhiteHSideCastleFR, 0, ff, 0, ft, closure);

            ft = castlingRights[1]; /* Rook file if we have A-side rights */
            left  = BOARD_LEFT+2;
            right = ff-1;
            if(ff <= BOARD_LEFT+2) { left = ff+1; right = BOARD_LEFT+3; }
            for(k=left; k<=right && ft >= 0; k++) /* first test if blocked */
                if(k != ft && board[0][k] != EmptySquare) ft = -1;
            if(ff > BOARD_LEFT+2) 
            for(k=left+1; k<=right && ft >= 0; k++) /* then if not checked */
                if(!ignoreCheck && CheckTest(board, flags, 0, ff, 0, k, FALSE)) ft = -1;
            if(ft >= 0 && board[0][ft] == WhiteRook)
                callback(board, flags, WhiteASideCastleFR, 0, ff, 0, ft, closure);
        }
    } else {
        ff = castlingRights[5]; /* King file if we have any rights */
        if(ff > 0 && board[BOARD_HEIGHT-1][ff] == BlackKing) {
            ft = castlingRights[3]; /* Rook file if we have H-side rights */
            left  = ff+1;
            right = BOARD_RGHT-2;
            if(ff == BOARD_RGHT-2) left = right = ff-1;    /* special case */
            for(k=left; k<=right && ft >= 0; k++) /* first test if blocked */
                if(k != ft && board[BOARD_HEIGHT-1][k] != EmptySquare) ft = -1;
            for(k=left; k<right && ft >= 0; k++) /* then if not checked */
                if(!ignoreCheck && CheckTest(board, flags, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, k, FALSE)) ft = -1;
            if(ft >= 0 && board[BOARD_HEIGHT-1][ft] == BlackRook)
                callback(board, flags, BlackHSideCastleFR, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, ft, closure);

            ft = castlingRights[4]; /* Rook file if we have A-side rights */
            left  = BOARD_LEFT+2;
            right = ff-1;
            if(ff <= BOARD_LEFT+2) { left = ff+1; right = BOARD_LEFT+3; }
            for(k=left; k<=right && ft >= 0; k++) /* first test if blocked */
                if(k != ft && board[BOARD_HEIGHT-1][k] != EmptySquare) ft = -1;
            if(ff > BOARD_LEFT+2) 
            for(k=left+1; k<=right && ft >= 0; k++) /* then if not checked */
                if(!ignoreCheck && CheckTest(board, flags, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, k, FALSE)) ft = -1;
            if(ft >= 0 && board[BOARD_HEIGHT-1][ft] == BlackRook)
                callback(board, flags, BlackASideCastleFR, BOARD_HEIGHT-1, ff, BOARD_HEIGHT-1, ft, closure);
        }
    }

  }

    return FALSE;
}


typedef struct {
    int rking, fking;
    int check;
} CheckTestClosure;


extern void CheckTestCallback P((Board board, int flags, ChessMove kind,
				 int rf, int ff, int rt, int ft,
				 VOIDSTAR closure));


void CheckTestCallback(board, flags, kind, rf, ff, rt, ft, closure)
     Board board;
     int flags;
     ChessMove kind;
     int rf, ff, rt, ft;
     VOIDSTAR closure;
{
    register CheckTestClosure *cl = (CheckTestClosure *) closure;

    if (rt == cl->rking && ft == cl->fking) cl->check++;
}


/* If the player on move were to move from (rf, ff) to (rt, ft), would
   he leave himself in check?  Or if rf == -1, is the player on move
   in check now?  enPassant must be TRUE if the indicated move is an
   e.p. capture.  The possibility of castling out of a check along the
   back rank is not accounted for (i.e., we still return nonzero), as
   this is illegal anyway.  Return value is the number of times the
   king is in check. */ 
int CheckTest(board, flags, rf, ff, rt, ft, enPassant)
     Board board;
     int flags;
     int rf, ff, rt, ft, enPassant;
{
    CheckTestClosure cl;
    ChessSquare king = flags & F_WHITE_ON_MOVE ? WhiteKing : BlackKing;
    ChessSquare captured = EmptySquare;
    /*  Suppress warnings on uninitialized variables    */

    if(gameInfo.variant == VariantXiangqi)
        king = flags & F_WHITE_ON_MOVE ? WhiteWazir : BlackWazir;
    if(gameInfo.variant == VariantKnightmate)
        king = flags & F_WHITE_ON_MOVE ? WhiteUnicorn : BlackUnicorn;

    if (rf >= 0) {
	if (enPassant) {
	    captured = board[rf][ft];
	    board[rf][ft] = EmptySquare;
	} else {
	    captured = board[rt][ft];
	}
	board[rt][ft] = board[rf][ff];
	board[rf][ff] = EmptySquare;
    }

    /* For compatibility with ICS wild 9, we scan the board in the
       order a1, a2, a3, ... b1, b2, ..., h8 to find the first king,
       and we test only whether that one is in check. */
    cl.check = 0;
    for (cl.fking = BOARD_LEFT+0; cl.fking < BOARD_RGHT; cl.fking++)
	for (cl.rking = 0; cl.rking < BOARD_HEIGHT; cl.rking++) {
          if (board[cl.rking][cl.fking] == king) {
              if(gameInfo.variant == VariantXiangqi) {
                  /* [HGM] In Xiangqi opposing Kings means check as well */
                  int i, dir;
                  dir = (king >= BlackPawn) ? -1 : 1;
                  for( i=cl.rking+dir; i>=0 && i<BOARD_HEIGHT &&
                                board[i][cl.fking] == EmptySquare; i+=dir );
                  if(i>=0 && i<BOARD_HEIGHT &&
                      board[i][cl.fking] == (dir>0 ? BlackWazir : WhiteWazir) )
                          cl.check++;
              }

	      GenPseudoLegal(board, flags ^ F_WHITE_ON_MOVE, -1,
			     CheckTestCallback, (VOIDSTAR) &cl);
	      goto undo_move;  /* 2-level break */
	  }
      }

  undo_move:

    if (rf >= 0) {
	board[rf][ff] = board[rt][ft];
	if (enPassant) {
	    board[rf][ft] = captured;
	    board[rt][ft] = EmptySquare;
	} else {
	    board[rt][ft] = captured;
	}
    }

    return cl.fking < BOARD_RGHT ? cl.check : 1000; // [HGM] atomic: return 1000 if we have no king
}


extern void LegalityTestCallback P((Board board, int flags, ChessMove kind,
				    int rf, int ff, int rt, int ft,
				    VOIDSTAR closure));

void LegalityTestCallback(board, flags, kind, rf, ff, rt, ft, closure)
     Board board;
     int flags;
     ChessMove kind;
     int rf, ff, rt, ft;
     VOIDSTAR closure;
{
    register LegalityTestClosure *cl = (LegalityTestClosure *) closure;

    if(board[rt][ft] != EmptySquare || kind==WhiteCapturesEnPassant || kind==BlackCapturesEnPassant)
	cl->captures++; // [HGM] losers: count legal captures
    if (rf == cl->rf && ff == cl->ff && rt == cl->rt && ft == cl->ft)
      cl->kind = kind;
}

ChessMove LegalityTest(board, flags, epfile, castlingRights, rf, ff, rt, ft, promoChar)
     Board board;
     int flags, epfile;
     int rf, ff, rt, ft, promoChar;
     char castlingRights[];
{
    LegalityTestClosure cl; ChessSquare piece = board[rf][ff];
    
    /* [HGM] Lance, Cobra and Falcon are wildcard pieces; consider all their moves legal */
    /* (perhaps we should disallow moves that obviously leave us in check?)              */
    if(piece == WhiteFalcon || piece == BlackFalcon ||
       piece == WhiteCobra  || piece == BlackCobra  ||
       piece == WhiteLance  || piece == BlackLance)
        return CheckTest(board, flags, rf, ff, rt, ft, FALSE) ? IllegalMove : NormalMove;

    cl.rf = rf;
    cl.ff = ff;
    cl.rt = rt;
    cl.ft = ft;
    cl.kind = IllegalMove;
    cl.captures = 0; // [HGM] losers: prepare to count legal captures.
    GenLegal(board, flags, epfile, castlingRights, LegalityTestCallback, (VOIDSTAR) &cl);
    if((flags & F_MANDATORY_CAPTURE) && cl.captures && board[rt][ft] == EmptySquare
		&& cl.kind != WhiteCapturesEnPassant && cl.kind != BlackCapturesEnPassant)
	return(IllegalMove); // [HGM] losers: if there are legal captures, non-capts are illegal

    if(gameInfo.variant == VariantShogi) {
        /* [HGM] Shogi promotions. '=' means defer */
        if(rf != DROP_RANK && cl.kind == NormalMove) {
            ChessSquare piece = board[rf][ff];

            if(promoChar == PieceToChar(BlackQueen)) promoChar = NULLCHAR; /* [HGM] Kludge */
            if(promoChar != NULLCHAR && promoChar != 'x' &&
               promoChar != '+' && promoChar != '=' &&
               ToUpper(PieceToChar(PROMOTED piece)) != ToUpper(promoChar) )
                    cl.kind = IllegalMove;
            else if(flags & F_WHITE_ON_MOVE) {
                if( (int) piece < (int) WhiteWazir &&
                     (rf > BOARD_HEIGHT-4 || rt > BOARD_HEIGHT-4) ) {
                    if( (piece == WhitePawn || piece == WhiteQueen) && rt > BOARD_HEIGHT-2 ||
                         piece == WhiteKnight && rt > BOARD_HEIGHT-3) /* promotion mandatory */
                             cl.kind = promoChar == '=' ? IllegalMove : WhitePromotionKnight;
                    else /* promotion optional, default is promote */
                             cl.kind = promoChar == '=' ? NormalMove  : WhitePromotionQueen;
                   
                } else cl.kind = (promoChar == NULLCHAR || promoChar == 'x' || promoChar == '=') ?
                                            NormalMove : IllegalMove;
            } else {
                if( (int) piece < (int) BlackWazir && (rf < 3 || rt < 3) ) {
                    if( (piece == BlackPawn || piece == BlackQueen) && rt < 1 ||
                         piece == BlackKnight && rt < 2 ) /* promotion obligatory */
                             cl.kind = promoChar == '=' ? IllegalMove : BlackPromotionKnight;
                    else /* promotion optional, default is promote */
                             cl.kind = promoChar == '=' ? NormalMove  : BlackPromotionQueen;

                } else cl.kind = (promoChar == NULLCHAR || promoChar == 'x' || promoChar == '=') ?
                                            NormalMove : IllegalMove;
            }
        }
    } else
    if (promoChar != NULLCHAR && promoChar != 'x') {
	if(promoChar == '=') cl.kind = IllegalMove; else // [HGM] shogi: no deferred promotion outside Shogi
	if (cl.kind == WhitePromotionQueen || cl.kind == BlackPromotionQueen) {
	    cl.kind = 
	      PromoCharToMoveType((flags & F_WHITE_ON_MOVE) != 0, promoChar);
	} else {
	    cl.kind = IllegalMove;
	}
    }
    /* [HGM] For promotions, 'ToQueen' = optional, 'ToKnight' = mandatory */
    return cl.kind;
}

typedef struct {
    int count;
} MateTestClosure;

extern void MateTestCallback P((Board board, int flags, ChessMove kind,
				int rf, int ff, int rt, int ft,
				VOIDSTAR closure));

void MateTestCallback(board, flags, kind, rf, ff, rt, ft, closure)
     Board board;
     int flags;
     ChessMove kind;
     int rf, ff, rt, ft;
     VOIDSTAR closure;
{
    register MateTestClosure *cl = (MateTestClosure *) closure;

    cl->count++;
}

/* Return MT_NONE, MT_CHECK, MT_CHECKMATE, or MT_STALEMATE */
int MateTest(board, flags, epfile, castlingRights)
     Board board;
     int flags, epfile;
     char castlingRights[];
{
    MateTestClosure cl;
    int inCheck, r, f, myPieces=0, hisPieces=0, nrKing=0;
    ChessSquare king = flags & F_WHITE_ON_MOVE ? WhiteKing : BlackKing;

    for(r=0; r<BOARD_HEIGHT; r++) for(f=BOARD_LEFT; f<BOARD_RGHT; f++) { 
        // [HGM] losers: Count pieces and kings, to detect other unorthodox winning conditions
	nrKing += (board[r][f] == king);   // stm has king
        if( board[r][f] != EmptySquare ) {
	    if((int)board[r][f] <= (int)king && (int)board[r][f] >= (int)king - (int)WhiteKing + (int)WhitePawn)
		 myPieces++;
	    else hisPieces++;
	}
    }

    switch(gameInfo.variant) { // [HGM] losers: extinction wins
	case VariantShatranj:
		if(hisPieces == 1) return myPieces > 1 ? MT_BARE : MT_DRAW;
	default:
		break;
	case VariantAtomic:
		if(nrKing == 0) return MT_NOKING;
		break;
	case VariantLosers:
		if(myPieces == 1) return MT_BARE;
    }
    cl.count = 0;
    inCheck = GenLegal(board, flags, epfile, castlingRights, MateTestCallback, (VOIDSTAR) &cl);
    // [HGM] 3check: yet to do!
    if (cl.count > 0) {
	return inCheck ? MT_CHECK : MT_NONE;
    } else {
	if(gameInfo.variant == VariantSuicide) // [HGM] losers: always stalemate, since no check, but result varies
		return myPieces == hisPieces ? MT_STALEMATE :
					myPieces > hisPieces ? MT_STAINMATE : MT_STEALMATE;
	else if(gameInfo.variant == VariantLosers) return inCheck ? MT_TRICKMATE : MT_STEALMATE;
	else if(gameInfo.variant == VariantGiveaway) return MT_STEALMATE; // no check exists, stalemated = win
					    
        return inCheck ? MT_CHECKMATE 
		       : (gameInfo.variant == VariantXiangqi || gameInfo.variant == VariantShatranj) ? 
			  MT_STAINMATE : MT_STALEMATE;
    }
}

     
extern void DisambiguateCallback P((Board board, int flags, ChessMove kind,
				    int rf, int ff, int rt, int ft,
				    VOIDSTAR closure));

void DisambiguateCallback(board, flags, kind, rf, ff, rt, ft, closure)
     Board board;
     int flags;
     ChessMove kind;
     int rf, ff, rt, ft;
     VOIDSTAR closure;
{
    register DisambiguateClosure *cl = (DisambiguateClosure *) closure;
    int wildCard = FALSE; ChessSquare piece = board[rf][ff];

    // [HGM] wild: for wild-card pieces rt and rf are dummies
    if(piece == WhiteFalcon || piece == BlackFalcon ||
       piece == WhiteCobra  || piece == BlackCobra  ||
       piece == WhiteLance  || piece == BlackLance)
        wildCard = TRUE;

    if ((cl->pieceIn == EmptySquare || cl->pieceIn == board[rf][ff]
         || PieceToChar(board[rf][ff]) == '~'
              && cl->pieceIn == (ChessSquare)(DEMOTED board[rf][ff])
                                                                      ) &&
	(cl->rfIn == -1 || cl->rfIn == rf) &&
	(cl->ffIn == -1 || cl->ffIn == ff) &&
	(cl->rtIn == -1 || cl->rtIn == rt || wildCard) &&
	(cl->ftIn == -1 || cl->ftIn == ft || wildCard)) {

	cl->count++;
        cl->piece = board[rf][ff];
	cl->rf = rf;
	cl->ff = ff;
	cl->rt = wildCard ? cl->rtIn : rt;
	cl->ft = wildCard ? cl->ftIn : ft;
	cl->kind = kind;
    }
}

void Disambiguate(board, flags, epfile, closure)
     Board board;
     int flags, epfile;
     DisambiguateClosure *closure;
{
    int illegal = 0; char c = closure->promoCharIn;

    closure->count = 0;
    closure->rf = closure->ff = closure->rt = closure->ft = 0;
    closure->kind = ImpossibleMove;

    GenLegal(board, flags, epfile, initialRights, DisambiguateCallback, (VOIDSTAR) closure);
    if (closure->count == 0) {
	/* See if it's an illegal move due to check */
        illegal = 1;
        GenLegal(board, flags|F_IGNORE_CHECK, epfile, initialRights, DisambiguateCallback,
		 (VOIDSTAR) closure);	
	if (closure->count == 0) {
	    /* No, it's not even that */
	    return;
	}
    }

    if(gameInfo.variant == VariantShogi) {
        /* [HGM] Shogi promotions. '=' means defer */
        if(closure->rfIn != DROP_RANK && closure->kind == NormalMove) {
            ChessSquare piece = closure->piece;
            if(c != NULLCHAR && c != 'x' && c != '+' && c != '=' &&
               ToUpper(PieceToChar(PROMOTED piece)) != ToUpper(c) ) 
                    closure->kind = IllegalMove;
            else if(flags & F_WHITE_ON_MOVE) {
                if( (int) piece < (int) WhiteWazir &&
                     (closure->rf > BOARD_HEIGHT-4 || closure->rt > BOARD_HEIGHT-4) ) {
                    if( (piece == WhitePawn || piece == WhiteQueen) && closure->rt > BOARD_HEIGHT-2 ||
                         piece == WhiteKnight && closure->rt > BOARD_HEIGHT-3) /* promotion mandatory */
                             closure->kind = c == '=' ? IllegalMove : WhitePromotionKnight;
                    else /* promotion optional, default is promote */
                             closure->kind = c == '=' ? NormalMove  : WhitePromotionQueen;
                   
                } else closure->kind = (c == NULLCHAR || c == 'x' || c == '=') ?
                                            NormalMove : IllegalMove;
            } else {
                if( (int) piece < (int) BlackWazir && (closure->rf < 3 || closure->rt < 3) ) {
                    if( (piece == BlackPawn || piece == BlackQueen) && closure->rt < 1 ||
                         piece == BlackKnight && closure->rt < 2 ) /* promotion obligatory */
                             closure->kind = c == '=' ? IllegalMove : BlackPromotionKnight;
                    else /* promotion optional, default is promote */
                             closure->kind = c == '=' ? NormalMove  : BlackPromotionQueen;

                } else closure->kind = (c == NULLCHAR || c == 'x' || c == '=') ?
                                            NormalMove : IllegalMove;
            }
        }
    } else
    if (closure->promoCharIn != NULLCHAR && closure->promoCharIn != 'x') {
	if (closure->kind == WhitePromotionQueen
	    || closure->kind == BlackPromotionQueen) {
	    closure->kind = 
	      PromoCharToMoveType((flags & F_WHITE_ON_MOVE) != 0,
				  closure->promoCharIn);
	} else {
	    closure->kind = IllegalMove;
	}
    }
    /* [HGM] returns 'q' for optional promotion, 'n' for mandatory */
    if(closure->promoCharIn != '=')
        closure->promoChar = ToLower(closure->promoCharIn);
    else closure->promoChar = '=';
    if (closure->promoChar == 'x') closure->promoChar = NULLCHAR;
    if (closure->count > 1) {
	closure->kind = AmbiguousMove;
    }
    if (illegal) {
	/* Note: If more than one illegal move matches, but no legal
	   moves, we return IllegalMove, not AmbiguousMove.  Caller
	   can look at closure->count to detect this.
	*/
	closure->kind = IllegalMove;
    }
    if(closure->kind == IllegalMove)
    /* [HGM] might be a variant we don't understand, pass on promotion info */
        closure->promoChar = ToLower(closure->promoCharIn);
}


typedef struct {
    /* Input */
    ChessSquare piece;
    int rf, ff, rt, ft;
    /* Output */
    ChessMove kind;
    int rank;
    int file;
    int either;
} CoordsToAlgebraicClosure;

extern void CoordsToAlgebraicCallback P((Board board, int flags,
					 ChessMove kind, int rf, int ff,
					 int rt, int ft, VOIDSTAR closure));

void CoordsToAlgebraicCallback(board, flags, kind, rf, ff, rt, ft, closure)
     Board board;
     int flags;
     ChessMove kind;
     int rf, ff, rt, ft;
     VOIDSTAR closure;
{
    register CoordsToAlgebraicClosure *cl =
      (CoordsToAlgebraicClosure *) closure;

    if (rt == cl->rt && ft == cl->ft &&
        (board[rf][ff] == cl->piece
         || PieceToChar(board[rf][ff]) == '~' &&
            (ChessSquare) (DEMOTED board[rf][ff]) == cl->piece)
                                     ) {
	if (rf == cl->rf) {
	    if (ff == cl->ff) {
		cl->kind = kind; /* this is the move we want */
	    } else {
		cl->file++; /* need file to rule out this move */
	    }
	} else {
	    if (ff == cl->ff) {
		cl->rank++; /* need rank to rule out this move */
	    } else {
		cl->either++; /* rank or file will rule out this move */
	    }
	}	    
    }
}

/* Convert coordinates to normal algebraic notation.
   promoChar must be NULLCHAR or 'x' if not a promotion.
*/
ChessMove CoordsToAlgebraic(board, flags, epfile,
			    rf, ff, rt, ft, promoChar, out)
     Board board;
     int flags, epfile;
     int rf, ff, rt, ft;
     int promoChar;
     char out[MOVE_LEN];
{
    ChessSquare piece;
    ChessMove kind;
    char *outp = out, c;
    CoordsToAlgebraicClosure cl;
    
    if (rf == DROP_RANK) {
	/* Bughouse piece drop */
	*outp++ = ToUpper(PieceToChar((ChessSquare) ff));
	*outp++ = '@';
        *outp++ = ft + AAA;
        if(rt+ONE <= '9')
           *outp++ = rt + ONE;
        else { *outp++ = (rt+ONE-'0')/10 + '0';*outp++ = (rt+ONE-'0')%10 + '0'; }
	*outp = NULLCHAR;
	return (flags & F_WHITE_ON_MOVE) ? WhiteDrop : BlackDrop;
    }

    if (promoChar == 'x') promoChar = NULLCHAR;
    piece = board[rf][ff];
    if(PieceToChar(piece)=='~') piece = (ChessSquare)(DEMOTED piece);

    switch (piece) {
      case WhitePawn:
      case BlackPawn:
        kind = LegalityTest(board, flags, epfile, initialRights, rf, ff, rt, ft, promoChar);
	if (kind == IllegalMove && !(flags&F_IGNORE_CHECK)) {
	    /* Keep short notation if move is illegal only because it
               leaves the player in check, but still return IllegalMove */
            kind = LegalityTest(board, flags|F_IGNORE_CHECK, epfile, initialRights,
			       rf, ff, rt, ft, promoChar);
	    if (kind == IllegalMove) break;
	    kind = IllegalMove;
	}
	/* Pawn move */
        *outp++ = ff + AAA;
        if (ff == ft && board[rt][ft] == EmptySquare) { /* [HGM] Xiangqi has straight noncapts! */
	    /* Non-capture; use style "e5" */
            if(rt+ONE <= '9')
               *outp++ = rt + ONE;
            else { *outp++ = (rt+ONE-'0')/10 + '0';*outp++ = (rt+ONE-'0')%10 + '0'; }
	} else {
	    /* Capture; use style "exd5" */
            if(gameInfo.variant != VariantXiangqi || board[rt][ft] != EmptySquare )
            *outp++ = 'x';  /* [HGM] Xiangqi has sideway noncaptures across river! */
            *outp++ = ft + AAA;
            if(rt+ONE <= '9')
               *outp++ = rt + ONE;
            else { *outp++ = (rt+ONE-'0')/10 + '0';*outp++ = (rt+ONE-'0')%10 + '0'; }
	}
	/* Use promotion suffix style "=Q" */
	*outp = NULLCHAR;
        if (promoChar != NULLCHAR) {
            if(gameInfo.variant == VariantShogi) {
                /* [HGM] ... but not in Shogi! */
                *outp++ = promoChar == '=' ? '=' : '+';
            } else {
                *outp++ = '=';
                *outp++ = ToUpper(promoChar);
            }
            *outp = NULLCHAR;
	}
        return kind;

	
      case WhiteKing:
      case BlackKing:
        /* Fabien moved code: FRC castling first (if KxR), wild castling second */
	/* Code added by Tord:  FRC castling. */
	if((piece == WhiteKing && board[rt][ft] == WhiteRook) ||
	   (piece == BlackKing && board[rt][ft] == BlackRook)) {
	  if(ft > ff) strcpy(out, "O-O"); else strcpy(out, "O-O-O");
            return LegalityTest(board, flags, epfile, initialRights,
				rf, ff, rt, ft, promoChar);
	}
	/* End of code added by Tord */
	/* Test for castling or ICS wild castling */
	/* Use style "O-O" (oh-oh) for PGN compatibility */
	else if (rf == rt &&
	    rf == ((piece == WhiteKing) ? 0 : BOARD_HEIGHT-1) &&
            ((ff == BOARD_WIDTH>>1 && (ft == BOARD_LEFT+2 || ft == BOARD_RGHT-2)) ||
             (ff == (BOARD_WIDTH-1)>>1 && (ft == BOARD_LEFT+1 || ft == BOARD_RGHT-3)))) {
            if(ft==BOARD_LEFT+1 || ft==BOARD_RGHT-2)
		strcpy(out, "O-O");
            else
		strcpy(out, "O-O-O");

	    /* This notation is always unambiguous, unless there are
	       kings on both the d and e files, with "wild castling"
	       possible for the king on the d file and normal castling
	       possible for the other.  ICS rules for wild 9
	       effectively make castling illegal for either king in
	       this situation.  So I am not going to worry about it;
	       I'll just generate an ambiguous O-O in this case.
	    */
            return LegalityTest(board, flags, epfile, initialRights,
				rf, ff, rt, ft, promoChar);
	}

	/* else fall through */
      default:
	/* Piece move */
	cl.rf = rf;
	cl.ff = ff;
	cl.rt = rt;
	cl.ft = ft;
	cl.piece = piece;
	cl.kind = IllegalMove;
	cl.rank = cl.file = cl.either = 0;
        GenLegal(board, flags, epfile, initialRights,
		 CoordsToAlgebraicCallback, (VOIDSTAR) &cl);

	if (cl.kind == IllegalMove && !(flags&F_IGNORE_CHECK)) {
	    /* Generate pretty moves for moving into check, but
	       still return IllegalMove.
	    */
            GenLegal(board, flags|F_IGNORE_CHECK, epfile, initialRights,
		     CoordsToAlgebraicCallback, (VOIDSTAR) &cl);
	    if (cl.kind == IllegalMove) break;
	    cl.kind = IllegalMove;
	}

	/* Style is "Nf3" or "Nxf7" if this is unambiguous,
	   else "Ngf3" or "Ngxf7",
	   else "N1f3" or "N5xf7",
	   else "Ng1f3" or "Ng5xf7".
	*/
        c = PieceToChar(piece) ;
        if( c == '~' || c == '+') {
           /* [HGM] print nonexistent piece as its demoted version */
           piece = (ChessSquare) (DEMOTED piece);
        }
        if(c=='+') *outp++ = c;
        *outp++ = ToUpper(PieceToChar(piece));

	if (cl.file || (cl.either && !cl.rank)) {
            *outp++ = ff + AAA;
	}
	if (cl.rank) {
            if(rf+ONE <= '9')
                *outp++ = rf + ONE;
            else { *outp++ = (rf+ONE-'0')/10 + '0';*outp++ = (rf+ONE-'0')%10 + '0'; }
	}

	if(board[rt][ft] != EmptySquare)
	  *outp++ = 'x';

        *outp++ = ft + AAA;
        if(rt+ONE <= '9')
           *outp++ = rt + ONE;
        else { *outp++ = (rt+ONE-'0')/10 + '0';*outp++ = (rt+ONE-'0')%10 + '0'; }
	*outp = NULLCHAR;
        if (gameInfo.variant == VariantShogi) {
            /* [HGM] in Shogi non-pawns can promote */
            if(flags & F_WHITE_ON_MOVE) {
                if( (int) cl.piece < (int) WhiteWazir &&
                     (rf > BOARD_HEIGHT-4 || rt > BOARD_HEIGHT-4) ) {
                    if( (piece == WhitePawn || piece == WhiteQueen) && rt > BOARD_HEIGHT-2 ||
                         piece == WhiteKnight && rt > BOARD_HEIGHT-3) /* promotion mandatory */
                             cl.kind = promoChar == '=' ? IllegalMove : WhitePromotionKnight;
                    else cl.kind =  WhitePromotionQueen; /* promotion optional */
                   
                } else cl.kind = (promoChar == NULLCHAR || promoChar == 'x' || promoChar == '=') ?
                                            NormalMove : IllegalMove;
            } else {
                if( (int) cl.piece < (int) BlackWazir && (rf < 3 || rt < 3) ) {
                    if( (piece == BlackPawn || piece == BlackQueen) && rt < 1 ||
                         piece == BlackKnight && rt < 2 ) /* promotion obligatory */
                             cl.kind = promoChar == '=' ? IllegalMove : BlackPromotionKnight;
                    else cl.kind =  BlackPromotionQueen; /* promotion optional */
                } else cl.kind = (promoChar == NULLCHAR || promoChar == 'x' || promoChar == '=') ?
                                            NormalMove : IllegalMove;
            }
            if(cl.kind == WhitePromotionQueen || cl.kind == BlackPromotionQueen) {
                /* for optional promotions append '+' or '=' */
                if(promoChar == '=') {
                    *outp++ = '=';
                    cl.kind = NormalMove;
                } else *outp++ = '+';
                *outp = NULLCHAR;
            } else if(cl.kind == IllegalMove) {
                /* Illegal move specifies any given promotion */
                if(promoChar != NULLCHAR && promoChar != 'x') {
                    *outp++ = '=';
                    *outp++ = ToUpper(promoChar);
                    *outp = NULLCHAR;
                }
            }
        }
        return cl.kind;
	
      /* [HGM] Always long notation for fairies we don't know */
      case WhiteFalcon:
      case BlackFalcon:
      case WhiteLance:
      case BlackLance:
      case WhiteGrasshopper:
      case BlackGrasshopper:
      case EmptySquare:
	/* Moving a nonexistent piece */
	break;
    }
    
    /* Not a legal move, even ignoring check.
       If there was a piece on the from square, 
       use style "Ng1g3" or "Ng1xe8";
       if there was a pawn or nothing (!),
       use style "g1g3" or "g1xe8".  Use "x"
       if a piece was on the to square, even
       a piece of the same color.
    */
    outp = out;
    if (piece != EmptySquare && piece != WhitePawn && piece != BlackPawn) {
	*outp++ = ToUpper(PieceToChar(piece));
    }
    *outp++ = ff + AAA;
    if(rf+ONE <= '9')
       *outp++ = rf + ONE;
    else { *outp++ = (rf+ONE-'0')/10 + '0';*outp++ = (rf+ONE-'0')%10 + '0'; }
    if (board[rt][ft] != EmptySquare) *outp++ = 'x';
    *outp++ = ft + AAA;
    if(rt+ONE <= '9')
       *outp++ = rt + ONE;
    else { *outp++ = (rt+ONE-'0')/10 + '0';*outp++ = (rt+ONE-'0')%10 + '0'; }
    /* Use promotion suffix style "=Q" */
    if (promoChar != NULLCHAR && promoChar != 'x') {
	*outp++ = '=';
	*outp++ = ToUpper(promoChar);
    }
    *outp = NULLCHAR;

    return IllegalMove;
}

// [HGM] XQ: the following code serves to detect perpetual chasing (Asian rules)

typedef struct {
    /* Input */
    int rf, ff, rt, ft;
    /* Output */
    int recaptures;
} ChaseClosure;

// I guess the following variables logically belong in the closure too, but I was too lazy and used globals

int preyStackPointer, chaseStackPointer;

struct {
char rf, ff, rt, ft;
} chaseStack[100];

struct {
char rank, file;
} preyStack[100];




// there are three new callbacks for use with GenLegal: for adding captures, deleting them, and finding a recapture

extern void AtacksCallback P((Board board, int flags, ChessMove kind,
				int rf, int ff, int rt, int ft,
				VOIDSTAR closure));

void AttacksCallback(board, flags, kind, rf, ff, rt, ft, closure)
     Board board;
     int flags;
     ChessMove kind;
     int rf, ff, rt, ft;
     VOIDSTAR closure;
{   // For adding captures that can lead to chase indictment to the chaseStack
    if(board[rt][ft] == EmptySquare) return;                               // non-capture
    if(board[rt][ft] == WhitePawn && rt <  BOARD_HEIGHT/2) return;         // Pawn before river can be chased
    if(board[rt][ft] == BlackPawn && rt >= BOARD_HEIGHT/2) return;         // Pawn before river can be chased
    if(board[rf][ff] == WhitePawn  || board[rf][ff] == BlackPawn)  return; // Pawns are allowed to chase
    if(board[rf][ff] == WhiteWazir || board[rf][ff] == BlackWazir) return; // King is allowed to chase
    // move cannot be excluded from being a chase trivially (based on attacker and victim); save it on chaseStack
    chaseStack[chaseStackPointer].rf = rf;
    chaseStack[chaseStackPointer].ff = ff;
    chaseStack[chaseStackPointer].rt = rt;
    chaseStack[chaseStackPointer].ft = ft;
    chaseStackPointer++;
}

extern void ExistingAtacksCallback P((Board board, int flags, ChessMove kind,
				int rf, int ff, int rt, int ft,
				VOIDSTAR closure));

void ExistingAttacksCallback(board, flags, kind, rf, ff, rt, ft, closure)
     Board board;
     int flags;
     ChessMove kind;
     int rf, ff, rt, ft;
     VOIDSTAR closure;
{   // for removing pre-exsting captures from the chaseStack, to be left with newly created ones
    int i;
    register ChaseClosure *cl = (ChaseClosure *) closure; //closure tells us the move played in the repeat loop

    if(board[rt][ft] == EmptySquare) return; // no capture
    if(rf == cl->rf && ff == cl->ff) { // attacks with same piece from new position are not considered new
	rf = cl->rt; ff = cl->ft;      // doctor their fromSquare so they will be recognized in chaseStack
    }
    // search move in chaseStack, and delete it if it occurred there (as we know now it is not a new capture)
    for(i=0; i<chaseStackPointer; i++) {
	if(chaseStack[i].rf == rf && chaseStack[i].ff == ff && 
	   chaseStack[i].rt == rt && chaseStack[i].ft == ft   ) { 
	    // move found on chaseStack, delete it by overwriting with move popped from top of chaseStack
	    chaseStack[i] = chaseStack[--chaseStackPointer];
	    break;
	}
    }
}

extern void ProtectedCallback P((Board board, int flags, ChessMove kind,
				int rf, int ff, int rt, int ft,
				VOIDSTAR closure));

void ProtectedCallback(board, flags, kind, rf, ff, rt, ft, closure)
     Board board;
     int flags;
     ChessMove kind;
     int rf, ff, rt, ft;
     VOIDSTAR closure;
{   // for determining if a piece (given through the closure) is protected
    register ChaseClosure *cl = (ChaseClosure *) closure; // closure tells us where to recapture

    if(rt == cl->rt && ft == cl->ft) cl->recaptures++;    // count legal recaptures to this square
    if(appData.debugMode && board[rt][ft] != EmptySquare)
	fprintf(debugFP, "try %c%c%c%c=%d\n", ff+AAA, rf+ONE,ft+AAA, rt+ONE, cl->recaptures);
}

extern char moveList[MAX_MOVES][MOVE_LEN];

int PerpetualChase(int first, int last)
{   // this routine detects if the side to move in the 'first' position is perpetually chasing (when not checking)
    int i, j, k, tail;
    ChaseClosure cl;
    ChessSquare captured;

    preyStackPointer = 0;        // clear stack of chased pieces
    for(i=first; i<last; i+=2) { // for all positions with same side to move
        if(appData.debugMode) fprintf(debugFP, "judge position %i\n", i);
	chaseStackPointer = 0;   // clear stack that is going to hold possible chases
	// determine all captures possible after the move, and put them on chaseStack
	GenLegal(boards[i+1], PosFlags(i), EP_NONE, initialRights, AttacksCallback, &cl);
	if(appData.debugMode) { int n; 
	    for(n=0; n<chaseStackPointer; n++) 
                fprintf(debugFP, "%c%c%c%c ", chaseStack[n].ff+AAA, chaseStack[n].rf+ONE, 
                                              chaseStack[n].ft+AAA, chaseStack[n].rt+ONE);
            fprintf(debugFP, ": all capts\n");
	}
	// determine all captures possible before the move, and delete them from chaseStack
	cl.rf = moveList[i][1]-ONE; // prepare closure to pass move that led from i to i+1
	cl.ff = moveList[i][0]-AAA+BOARD_LEFT;
	cl.rt = moveList[i][3]-ONE;
	cl.ft = moveList[i][2]-AAA+BOARD_LEFT;
	GenLegal(boards[i],   PosFlags(i), EP_NONE, initialRights, ExistingAttacksCallback, &cl);
	if(appData.debugMode) { int n; 
	    for(n=0; n<chaseStackPointer; n++) 
                fprintf(debugFP, "%c%c%c%c ", chaseStack[n].ff+AAA, chaseStack[n].rf+ONE, 
                                              chaseStack[n].ft+AAA, chaseStack[n].rt+ONE);
            fprintf(debugFP, ": new capts after %c%c%c%c\n", cl.ff+AAA, cl.rf+ONE, cl.ft+AAA, cl.rt+ONE);
	}
	// chaseSack now contains all captures made possible by the move
	for(j=0; j<chaseStackPointer; j++) { // run through chaseStack to identify true chases
            int attacker = (int)boards[i+1][chaseStack[j].rf][chaseStack[j].ff];
            int victim   = (int)boards[i+1][chaseStack[j].rt][chaseStack[j].ft];

	    if(attacker >= (int) BlackPawn) attacker = BLACK_TO_WHITE attacker; // convert to white, as piecee type
	    if(victim   >= (int) BlackPawn) victim   = BLACK_TO_WHITE victim;

	    if((attacker == WhiteKnight || attacker == WhiteCannon) && victim == WhiteRook) 
		continue; // C or H attack on R is always chase; leave on chaseStack

	    if(attacker == victim) {
                if(LegalityTest(boards[i+1], PosFlags(i+1), EP_NONE, initialRights, chaseStack[j].rt, 
                   chaseStack[j].ft, chaseStack[j].rf, chaseStack[j].ff, NULLCHAR) == NormalMove) {
			// we can capture back with equal piece, so this is no chase but a sacrifice
                        chaseStack[j] = chaseStack[--chaseStackPointer]; // delete the capture from the chaseStack
			j--; /* ! */ continue;
		}

	    }

	    // the attack is on a lower piece, or on a pinned or blocked equal one
            // test if the victim is protected by a true protector. First make the capture.
	    captured = boards[i+1][chaseStack[j].rt][chaseStack[j].ft];
	    boards[i+1][chaseStack[j].rt][chaseStack[j].ft] = boards[i+1][chaseStack[j].rf][chaseStack[j].ff];
	    boards[i+1][chaseStack[j].rf][chaseStack[j].ff] = EmptySquare;
	    // Then test if the opponent can recapture
	    cl.recaptures = 0;         // prepare closure to pass recapture square and count moves to it
	    cl.rt = chaseStack[j].rt;
	    cl.ft = chaseStack[j].ft;
	    if(appData.debugMode) {
            	fprintf(debugFP, "test if we can recapture %c%c\n", cl.ft+AAA, cl.rt+ONE);
	    }
            GenLegal(boards[i+1], PosFlags(i+1), EP_NONE, initialRights, ProtectedCallback, &cl); // try all moves
	    // unmake the capture
	    boards[i+1][chaseStack[j].rf][chaseStack[j].ff] = boards[i+1][chaseStack[j].rt][chaseStack[j].ft];
            boards[i+1][chaseStack[j].rt][chaseStack[j].ft] = captured;
	    // if a recapture was found, piece is protected, and we are not chasing it.
	    if(cl.recaptures) { // attacked piece was defended by true protector, no chase
		chaseStack[j] = chaseStack[--chaseStackPointer]; // so delete from chaseStack
		j--; /* ! */ 
	    }
	}
	// chaseStack now contains all moves that chased
	if(appData.debugMode) { int n; 
	    for(n=0; n<chaseStackPointer; n++) 
                fprintf(debugFP, "%c%c%c%c ", chaseStack[n].ff+AAA, chaseStack[n].rf+ONE, 
                                              chaseStack[n].ft+AAA, chaseStack[n].rt+ONE);
            fprintf(debugFP, ": chases\n");
	}
        if(i == first) { // copy all people chased by first move of repeat cycle to preyStack
	    for(j=0; j<chaseStackPointer; j++) {
                preyStack[j].rank = chaseStack[j].rt;
                preyStack[j].file = chaseStack[j].ft;
	    }
	    preyStackPointer = chaseStackPointer;
	}
	tail = 0;
        for(j=0; j<chaseStackPointer; j++) {
	    for(k=0; k<preyStackPointer; k++) {
		// search the victim of each chase move on the preyStack (first occurrence)
		if(chaseStack[j].ft == preyStack[k].file && chaseStack[j].rt == preyStack[k].rank ) {
		    if(k < tail) break; // piece was already identified as still being chased
		    preyStack[preyStackPointer] = preyStack[tail]; // move chased piece to bottom part of preyStack
		    preyStack[tail] = preyStack[k];                // by swapping
		    preyStack[k] = preyStack[preyStackPointer];
		    tail++;
		    break;
		}
	    }
	}
        preyStackPointer = tail; // keep bottom part of preyStack, popping pieces unchased on move i.
	if(appData.debugMode) { int n; 
            for(n=0; n<preyStackPointer; n++) 
                fprintf(debugFP, "%c%c ", preyStack[n].file+AAA, preyStack[n].rank+ONE);
            fprintf(debugFP, "always chased upto ply %d\n", i);
	}
        // now adjust the location of the chased pieces according to opponent move
        for(j=0; j<preyStackPointer; j++) {
            if(preyStack[j].rank == moveList[i+1][1]-ONE &&
               preyStack[j].file == moveList[i+1][0]-AAA+BOARD_LEFT) {
                preyStack[j].rank = moveList[i+1][3]-ONE;
                preyStack[j].file = moveList[i+1][2]-AAA+BOARD_LEFT;
                break;
            }
        }
    }
    return preyStackPointer; // if any piece was left on preyStack, it has been perpetually chased
}
