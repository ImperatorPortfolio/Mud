/***************************************************************************
 * Zero Point - Attribute Training
 *
 * Progress-based deliberate training built on the D20 ability and trait
 * potential systems. The inherited binary training implementation remains
 * compiled under a legacy symbol during this bounded migration.
 ***************************************************************************/

#include "mud.h"

struct training_session_data
{
   char attribute[16];
   short ability;
   short trainer_modifier;
   short exertion;
};

void apply_exertion_nutrition(
   CHAR_DATA *ch,
   int exertion )
{
   if( !ch
       || IS_NPC( ch )
       || !ch->pcdata )
      return;

   switch( exertion )
   {
      case EXERTION_LIGHT:
         if( number_range( 1, 4 ) == 1 )
            consume_nutrition_values(
               ch,
               0, 1, 0, 0, 0, 1 );
         break;

      case EXERTION_MODERATE:
         consume_nutrition_values(
            ch,
            0, 1, 0, 0, 0, 1 );
         break;

      case EXERTION_HEAVY:
         consume_nutrition_values(
            ch,
            1, 2, 0, 0, 1, 2 );
         break;

      case EXERTION_EXTREME:
         consume_nutrition_values(
            ch,
            1, 3, 1, 0, 1, 3 );
         break;

      default:
         break;
   }
}

void do_train( CHAR_DATA *ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *mob;
   int effect;
   int ability;
   int trainer_modifier;
   int session_exertion;
   bool improved;

   if( IS_NPC( ch ) || !ch->pcdata )
      return;

   strlcpy( arg, argument, MAX_INPUT_LENGTH );

   switch( ch->substate )
   {
      default:
      {
         struct training_session_data *session;

         if( arg[0] == '\0' )
         {
            send_to_char( "Train what?\r\n", ch );
            send_to_char(
               "\r\nChoices: strength, intelligence, wisdom, "
               "dexterity, constitution or charisma\r\n",
               ch );
            return;
         }

         if( !IS_AWAKE( ch ) )
         {
            send_to_char( "In your dreams, or what?\r\n", ch );
            return;
         }

         if( !ch->in_room )
            return;

         mob = NULL;

         for( mob = ch->in_room->first_person;
              mob;
              mob = mob->next_in_room )
         {
            if( IS_NPC( mob )
                && IS_SET( mob->act, ACT_TRAIN ) )
               break;
         }

         if( !mob )
         {
            send_to_char( "You can't do that here.\r\n", ch );
            return;
         }

         effect = TRAIT_EFFECT_NONE;
         ability = ABILITY_SCORE_STR;

         if( !str_cmp( arg, "str" )
             || !str_cmp( arg, "strength" ) )
         {
            effect = TRAIT_EFFECT_STR_POTENTIAL;
            ability = ABILITY_SCORE_STR;
            strlcpy( arg, "strength", MAX_INPUT_LENGTH );

            send_to_char(
               "&GYou begin your weight training.\r\n",
               ch );
         }
         else if( !str_cmp( arg, "dex" )
                  || !str_cmp( arg, "dexterity" ) )
         {
            effect = TRAIT_EFFECT_DEX_POTENTIAL;
            ability = ABILITY_SCORE_DEX;
            strlcpy( arg, "dexterity", MAX_INPUT_LENGTH );

            send_to_char(
               "&GYou begin to work at some challenging tests "
               "of coordination.\r\n",
               ch );
         }
         else if( !str_cmp( arg, "int" )
                  || !str_cmp( arg, "intelligence" ) )
         {
            effect = TRAIT_EFFECT_INT_POTENTIAL;
            ability = ABILITY_SCORE_INT;
            strlcpy( arg, "intelligence", MAX_INPUT_LENGTH );

            send_to_char(
               "&GYou begin your studies.\r\n",
               ch );
         }
         else if( !str_cmp( arg, "wis" )
                  || !str_cmp( arg, "wisdom" ) )
         {
            effect = TRAIT_EFFECT_WIS_POTENTIAL;
            ability = ABILITY_SCORE_WIS;
            strlcpy( arg, "wisdom", MAX_INPUT_LENGTH );

            send_to_char(
               "&GYou begin contemplating several ancient texts "
               "in an effort to gain wisdom.\r\n",
               ch );
         }
         else if( !str_cmp( arg, "con" )
                  || !str_cmp( arg, "constitution" ) )
         {
            effect = TRAIT_EFFECT_CON_POTENTIAL;
            ability = ABILITY_SCORE_CON;
            strlcpy( arg, "constitution", MAX_INPUT_LENGTH );

            send_to_char(
               "&GYou begin your endurance training.\r\n",
               ch );
         }
         else if( !str_cmp( arg, "cha" )
                  || !str_cmp( arg, "charisma" ) )
         {
            effect = TRAIT_EFFECT_CHA_POTENTIAL;
            ability = ABILITY_SCORE_CHA;
            strlcpy( arg, "charisma", MAX_INPUT_LENGTH );

            send_to_char(
               "&GYou begin lessons in manners and etiquette.\r\n",
               ch );
         }
         else
         {
            do_train( ch, "" );
            return;
         }

         trainer_modifier =
            get_trait_training_chance(
               ch,
               mob,
               effect );

         if( trainer_modifier <= 0 )
         {
            int potential;

            potential =
               get_trait_training_potential(
                  ch,
                  effect );

            ch_printf(
               ch,
               "&YYou have reached your natural potential "
               "for this attribute: %d.&w\r\n",
               potential );
            return;
         }

         /*
          * The inherited helper returns a training chance. Preserve its
          * trainer/potential judgement, but convert a positive result into
          * a bounded productivity modifier for persistent progress.
          */
         trainer_modifier =
            URANGE(
               50,
               50 + trainer_modifier,
               150 );

         session_exertion =
            ( ability == ABILITY_SCORE_STR
              || ability == ABILITY_SCORE_DEX
              || ability == ABILITY_SCORE_CON )
            ? EXERTION_HEAVY
            : EXERTION_LIGHT;

         CREATE(
            session,
            struct training_session_data,
            1 );

         strlcpy(
            session->attribute,
            arg,
            sizeof( session->attribute ) );

         session->ability = ability;
         session->trainer_modifier = trainer_modifier;
         session->exertion = session_exertion;

         ch->dest_buf = session;

         add_timer(
            ch,
            TIMER_DO_FUN,
            10,
            do_train,
            1 );
         return;
      }

      case 1:
      {
         struct training_session_data *session;

         if( !ch->dest_buf )
            return;

         session =
            ( struct training_session_data * )ch->dest_buf;

         strlcpy(
            arg,
            session->attribute,
            MAX_INPUT_LENGTH );

         ability = session->ability;
         trainer_modifier = session->trainer_modifier;
         session_exertion = session->exertion;

         DISPOSE( ch->dest_buf );
         break;
      }

      case SUB_TIMER_DO_ABORT:
         DISPOSE( ch->dest_buf );
         ch->substate = SUB_NONE;

         send_to_char(
            "&RYou fail to complete your training.\r\n",
            ch );
         return;
   }

   ch->substate = SUB_NONE;

   apply_exertion_nutrition(
      ch,
      session_exertion );

   improved =
      gain_ability_training_progress(
         ch,
         ability,
         10,
         100,
         trainer_modifier );

   if( improved )
   {
      ch_printf(
         ch,
         "&GYour %s training pays off. "
         "Your ability has improved.&w\r\n",
         arg );
   }
   else
   {
      ch_printf(
         ch,
         "&GYou complete a productive %s training session.&w\r\n",
         arg );
   }
}
