/*
 * "Software pw3270, desenvolvido com base nos códigos fontes do WC3270  e X3270
 * (Paul Mattes Paul.Mattes@usa.net), de emulação de terminal 3270 para acesso a
 * aplicativos mainframe. Registro no INPI sob o nome G3270. Registro no INPI sob
 * o nome G3270.
 *
 * Copyright (C) <2008> <Banco do Brasil S.A.>
 *
 * Este programa é software livre. Você pode redistribuí-lo e/ou modificá-lo sob
 * os termos da GPL v.2 - Licença Pública Geral  GNU,  conforme  publicado  pela
 * Free Software Foundation.
 *
 * Este programa é distribuído na expectativa de  ser  útil,  mas  SEM  QUALQUER
 * GARANTIA; sem mesmo a garantia implícita de COMERCIALIZAÇÃO ou  de  ADEQUAÇÃO
 * A QUALQUER PROPÓSITO EM PARTICULAR. Consulte a Licença Pública Geral GNU para
 * obter mais detalhes.
 *
 * Você deve ter recebido uma cópia da Licença Pública Geral GNU junto com este
 * programa; se não, escreva para a Free Software Foundation, Inc., 51 Franklin
 * St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Este programa está nomeado como inout.c e possui - linhas de código.
 *
 * Contatos:
 *
 * perry.werneck@gmail.com	(Alexandre Perry de Souza Werneck)
 * erico.mendonca@gmail.com	(Erico Mascarenhas Mendonça)
 *
 */


 /**
  * @brief Pack/Unpack IPC data block.
  *
  */

 #include <config.h>
 #include <ipc-glib.h>
 #include <lib3270/log.h>
 #include <string.h>
 #include <stdlib.h>

/*---[ Implement ]----------------------------------------------------------------------------------*/

unsigned char * ipc3270_pack_error(const GError *error, size_t * szPacket) {

	static const char * error_response = "error";

	debug("%s(%d,%s)",__FUNCTION__,(int) error->code, (const char *) error->message);

	*szPacket = strlen(error_response) + 1 + (sizeof(guint16) * 2) + strlen(error->message);

	// Allocate buffer
	unsigned char * outputBuffer 	= g_malloc0(*szPacket);
	unsigned char * txtptr			= outputBuffer;

	// Add name
	strcpy((char *) txtptr,error_response);
	txtptr += strlen((char *) txtptr) + 1;

	// Add RC
	*((guint16 *) txtptr) = (guint16) (error->code ? error->code : -1);
	txtptr += sizeof(guint16);

	// Add message
	strcpy((char *) txtptr,error->message);
	txtptr += (strlen((char *) txtptr)+1);

	return outputBuffer;
}

static unsigned char * setup_header(unsigned char *txtptr, const gchar *name, int id, size_t count) {

	// Add name
	strcpy((char *) txtptr,name);
	txtptr += strlen((char *) txtptr) + 1;

	// Add ID
	*((guint16 *) txtptr) = (guint16) id;
	txtptr += sizeof(guint16);

	// Update ptr;
	*((guint16 *) txtptr) = (guint16) count;
	txtptr += sizeof(guint16);

	return txtptr;
}

unsigned char * pack_value(unsigned char *txtptr, GVariant *value) {

	const GVariantType *type = g_variant_get_type(value);

	if(g_variant_type_equal(type,G_VARIANT_TYPE_STRING)) {

		*(txtptr++) = 's';
		strcpy((char *) txtptr,g_variant_get_string(value,NULL));
		txtptr += (strlen((char *) txtptr)+1);

		debug("%s(%s)",__FUNCTION__,g_variant_get_string(value,NULL));

	} else if(g_variant_type_equal(type,G_VARIANT_TYPE_BOOLEAN)) {

		*(txtptr++) = 'b';
		*(txtptr++) = g_variant_get_boolean(value) ? 1 : 0;

		debug("%s(%s)",__FUNCTION__,g_variant_get_boolean(value) ? "True" : "False");

	} else if(g_variant_type_equal(type,G_VARIANT_TYPE_BYTE)) {

		*(txtptr++) = 'y';
		*(txtptr++) = g_variant_get_byte(value);

		debug("%s(%d)",__FUNCTION__,(int) g_variant_get_byte(value));

	} else if(g_variant_type_equal(type,G_VARIANT_TYPE_INT16)) {

		*(txtptr++) = 'n';
		*((gint16 *) txtptr) = g_variant_get_int16(value);
		txtptr += sizeof(gint16);

		debug("%s(%d)",__FUNCTION__,(int) g_variant_get_int16(value));

	} else if(g_variant_type_equal(type,G_VARIANT_TYPE_UINT16)) {

		*(txtptr++) = 'q';
		*((guint16 *) txtptr) = g_variant_get_uint16(value);
		txtptr += sizeof(guint16);

		debug("%s(%d)",__FUNCTION__,(int) g_variant_get_uint16(value));

	} else if(g_variant_type_is_subtype_of(type,G_VARIANT_TYPE_INT32)) {

		*(txtptr++) = 'i';
		*((gint32 *) txtptr) = g_variant_get_int32(value);
		txtptr += sizeof(gint32);

		debug("%s(%d)",__FUNCTION__,(int) g_variant_get_int32(value));

	} else if(g_variant_type_equal(type,G_VARIANT_TYPE_UINT32)) {

		*(txtptr++) = 'u';
		*((guint32 *) txtptr) = g_variant_get_uint32(value);
		txtptr += sizeof(guint32);

		debug("%s(%d)",__FUNCTION__,(int) g_variant_get_uint32(value));

	} else if(g_variant_type_equal(type,G_VARIANT_TYPE_INT64)) {

		*(txtptr++) = 'x';
		*((gint64 *) txtptr) = g_variant_get_int64(value);
		txtptr += sizeof(gint64);

	} else if(g_variant_type_equal(type,G_VARIANT_TYPE_UINT64)) {

		*(txtptr++) = 't';
		*((guint64 *) txtptr) = g_variant_get_uint64(value);
		txtptr += sizeof(guint64);

	} else {

		errno = EINVAL;
		g_message("Invalid IPC value type \"%s\"", g_variant_get_type_string(value));
		return NULL;

	}

	return txtptr;

}

unsigned char * ipc3270_pack(const gchar *name, GObject *object, int id, size_t * szPacket) {

	const GList	* node;
	size_t vCount = 0;

	g_return_val_if_fail(IS_IPC3270_RESPONSE(object),NULL);

	// Set packet size.
	*szPacket =
		strlen(name) + 1
		+ (sizeof(guint16) * 2);

	for(node = ipc3270_get_values(object); node; node = g_list_next(node)) {
		vCount++;
		*szPacket += g_variant_get_size(node->data)+1;
	}

	unsigned char * outputBuffer = g_malloc0(*szPacket);
	unsigned char * txtptr = setup_header(outputBuffer,name,id,vCount);

	for(node = ipc3270_get_values(object); node; node = g_list_next(node)) {
		txtptr = pack_value(txtptr, node->data);
		if(!txtptr) {
			g_free(outputBuffer);
			return NULL;
		}
	}

	debug("used=%u allocated=%u",(unsigned int) (txtptr-outputBuffer), (unsigned int) *szPacket);
	return outputBuffer;

}

GVariant * ipc3270_unpack(const unsigned char *packet, int *id) {

	const unsigned char *txtptr;
	size_t ix;

	// Skip packet name.
	packet += strlen((const char *) packet)+1;

	// Get Packet ID or error code.
	debug("Request type is %u",(unsigned int) *((guint16 *) packet));

	if(id) {
		*id = (int) *((guint16 *) packet);
	}
	packet += sizeof(guint16);

	// Get argument count.
	size_t arguments = (size_t) * ((guint16 *) packet);
	packet += sizeof(guint16);

	debug("Unpacking package with %u itens", (unsigned int) arguments);

	// Scan for argument types.
	g_autofree gchar * descrs = g_malloc0(arguments+3);
	descrs[0] = '(';
	descrs[arguments+1] = ')';

	txtptr = packet;
	for(ix = 0; ix < arguments; ix++) {

		debug("Format(%u): %c",(unsigned int) ix, *txtptr);

		descrs[ix+1] = *(txtptr++);

		switch(descrs[ix+1]) {
		case 's':
			txtptr += strlen((char *) txtptr)+1;
			break;

		case 'b':	//	gboolean
		case 'y':	//	guchar
			txtptr++;
			break;

		case 'n':	//	gint16
			txtptr += sizeof(gint16);
			break;

		case 'q':	//	guint16
			txtptr += sizeof(guint16);
			break;

		case 'i':	//	gint32
		case 'h':	//	gint32
			txtptr += sizeof(gint32);
			break;

		case 'u':	//	guint32
			txtptr += sizeof(guint32);
			break;

		case 'x':	//	gint64
			txtptr += sizeof(gint64);
			break;

		case 't':	//	guint64
			txtptr += sizeof(guint64);
			break;

		default:
			g_message("Unexpected format for argument %d: \"%c\"",(int) ix,descrs[ix+1]);
			errno = EINVAL;
			return NULL;
		}
	}

	debug("Format: \"%s\"\n",descrs);

	GVariantBuilder builder;
	g_variant_builder_init(&builder,G_VARIANT_TYPE(descrs));

	txtptr = packet;
	for(ix = 0; ix < arguments; ix++) {

		debug("Format(%u): %c",(unsigned int) ix, *txtptr);
		txtptr++;

		switch(descrs[ix+1]) {
		case 's':
			g_variant_builder_add(&builder, "s", txtptr);
			txtptr += strlen((char *) txtptr)+1;
			break;

		case 'b':	//	gboolean
			g_variant_builder_add(&builder, "b", *((gboolean *) txtptr));
			txtptr++;
			break;

		case 'y':	//	guchar
			g_variant_builder_add(&builder, "y", *((guchar *) txtptr));
			txtptr++;
			break;

		case 'n':	//	gint16
			g_variant_builder_add(&builder, "n", *((gint16 *) txtptr));
			txtptr += sizeof(gint16);
			break;

		case 'q':	//	guint16
			g_variant_builder_add(&builder, "q", *((guint16 *) txtptr));
			txtptr += sizeof(guint16);
			break;

		case 'i':	//	gint32
			g_variant_builder_add(&builder, "i", *((gint32 *) txtptr));
			txtptr += sizeof(gint32);
			break;

		case 'h':	//	gint32
			g_variant_builder_add(&builder, "h", *((gint32 *) txtptr));
			txtptr += sizeof(gint32);
			break;

		case 'u':	//	guint32
			g_variant_builder_add(&builder, "u", *((guint32 *) txtptr));
			txtptr += sizeof(guint32);
			break;

		case 'x':	//	gint64
			g_variant_builder_add(&builder, "x", *((gint64 *) txtptr));
			txtptr += sizeof(gint64);
			break;

		case 't':	//	guint64
			g_variant_builder_add(&builder, "t", *((guint64 *) txtptr));
			txtptr += sizeof(guint64);
			break;

		default:
			errno = EINVAL;
			return NULL;
		}
	}

	return g_variant_builder_end(&builder);

}

